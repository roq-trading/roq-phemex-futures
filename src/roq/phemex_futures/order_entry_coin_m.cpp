/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/phemex_futures/order_entry_coin_m.hpp"

#include <utility>

#include "roq/mask.hpp"

#include "roq/utils/safe_cast.hpp"
#include "roq/utils/update.hpp"

#include "roq/utils/metrics/factory.hpp"

#include "roq/server/oms/exceptions.hpp"

#include "roq/phemex_futures/json/encoder.hpp"
#include "roq/phemex_futures/json/map.hpp"
#include "roq/phemex_futures/json/utils.hpp"

using namespace std::literals;

namespace roq {
namespace phemex_futures {

// === CONSTANTS ===

namespace {
auto const NAME = "om"sv;

auto const SUPPORTS = Mask{
    SupportType::CREATE_ORDER,
    // SupportType::MODIFY_ORDER,
    SupportType::CANCEL_ORDER,
    SupportType::ORDER_ACK,
};

size_t const MAX_DECODE_BUFFER_DEPTH = 2;
}  // namespace

// === HELPERS ===

namespace {
auto create_name(auto stream_id, auto &account) {
  return fmt::format("{}:{}:{}"sv, stream_id, NAME, account.name);
}

auto create_connection(auto &handler, auto &settings, auto &context) {
  auto uri = settings.rest.uri;
  auto config = web::rest::Client::Config{
      // connection
      .interface = {},
      .proxy = settings.rest.proxy,
      .uris = {&uri, 1},
      .host = {},
      .validate_certificate = settings.net.tls_validate_certificate,
      // connection manager
      .connection_timeout = {},
      .disconnect_on_idle_timeout = {},
      .connection = web::http::Connection::KEEP_ALIVE,
      // request
      .allow_pipelining = true,
      .request_timeout = settings.rest.request_timeout,
      // response
      .suspend_on_retry_after = {},
      // http
      .query = {},
      .user_agent = ROQ_PACKAGE_NAME,
      .ping_frequency = settings.rest.ping_freq,
      .ping_path = settings.rest.ping_path,
      // implementation
      .decode_buffer_size = settings.misc.decode_buffer_size,
      .encode_buffer_size = settings.misc.encode_buffer_size,
  };
  return web::rest::Client::create(handler, context, config);
}

struct create_metrics final : public utils::metrics::Factory {
  create_metrics(auto &settings, auto &group, auto const &function) : utils::metrics::Factory{settings.app.name, group, function} {}
};
}  // namespace

// === IMPLEMENTATION ===

OrderEntryCoinM::OrderEntryCoinM(OrderEntry::Handler &handler, io::Context &context, uint16_t stream_id, Account &account, Shared &shared, bool master)
    : handler_{handler}, stream_id_{stream_id}, name_{create_name(stream_id_, account)}, master_{master},
      connection_{create_connection(*this, shared.settings, context)}, decode_buffer_{shared.settings.misc.decode_buffer_size, MAX_DECODE_BUFFER_DEPTH},
      counter_{
          .disconnect = create_metrics(shared.settings, name_, "disconnect"sv),
      },
      profile_{
          .orders_create = create_metrics(shared.settings, name_, "orders_create"sv),
          .orders_create_ack = create_metrics(shared.settings, name_, "orders_create_ack"sv),
          .orders_replace = create_metrics(shared.settings, name_, "orders_replace"sv),
          .orders_replace_ack = create_metrics(shared.settings, name_, "orders_replace_ack"sv),
          .orders_cancel = create_metrics(shared.settings, name_, "orders_cancel"sv),
          .orders_cancel_ack = create_metrics(shared.settings, name_, "orders_cancel_ack"sv),
          .orders_all = create_metrics(shared.settings, name_, "orders_all"sv),
          .orders_all_ack = create_metrics(shared.settings, name_, "orders_all_ack"sv),
      },
      latency_{
          .ping = create_metrics(shared.settings, name_, "ping"sv),
      },
      account_{account}, shared_{shared}, download_{shared.settings.rest.request_timeout, [this](auto state) { return download(state); }} {
}

bool OrderEntryCoinM::ready() const {
  return status_ == ConnectionStatus::READY;
}

void OrderEntryCoinM::operator()(Event<Start> const &) {
  (*connection_).start();
}

void OrderEntryCoinM::operator()(Event<Stop> const &) {
  (*connection_).stop();
}

void OrderEntryCoinM::operator()(Event<Timer> const &event) {
  auto now = event.value.now;
  (*connection_).refresh(now);
  if (!ready()) {
    return;
  }
}

void OrderEntryCoinM::operator()(metrics::Writer &writer) const {
  writer
      // counter
      .write(counter_.disconnect, metrics::Type::COUNTER)
      // profile
      .write(profile_.orders_create, metrics::Type::PROFILE)
      .write(profile_.orders_create_ack, metrics::Type::PROFILE)
      .write(profile_.orders_replace, metrics::Type::PROFILE)
      .write(profile_.orders_replace_ack, metrics::Type::PROFILE)
      .write(profile_.orders_cancel, metrics::Type::PROFILE)
      .write(profile_.orders_cancel_ack, metrics::Type::PROFILE)
      .write(profile_.orders_all, metrics::Type::PROFILE)
      .write(profile_.orders_all_ack, metrics::Type::PROFILE)
      // latency
      .write(latency_.ping, metrics::Type::LATENCY);
}

uint16_t OrderEntryCoinM::operator()(
    Event<CreateOrder> const &event, server::oms::Order const &order, server::oms::RefData const &ref_data, std::string_view const &request_id) {
  orders_create(event, order, ref_data, request_id);
  return stream_id_;
}

uint16_t OrderEntryCoinM::operator()(
    Event<ModifyOrder> const &,
    server::oms::Order const &,
    server::oms::RefData const &,
    [[maybe_unused]] std::string_view const &request_id,
    [[maybe_unused]] std::string_view const &previous_request_id) {
  throw server::oms::NotSupported{"not supported"sv};
  /*
  orders_replace(event, order, request_id, previous_request_id);
  return stream_id_;
  */
}

uint16_t OrderEntryCoinM::operator()(
    Event<CancelOrder> const &event,
    server::oms::Order const &order,
    server::oms::RefData const &ref_data,
    std::string_view const &request_id,
    std::string_view const &previous_request_id) {
  orders_cancel(event, order, ref_data, request_id, previous_request_id);
  return stream_id_;
}

uint16_t OrderEntryCoinM::operator()(Event<CancelAllOrders> const &event, std::string_view const &request_id) {
  orders_all(event, request_id);
  return stream_id_;
}

// web::rest::Client::Handler

void OrderEntryCoinM::operator()(Trace<web::rest::Client::Connected> const &) {
  if (download_.downloading()) {
    download_.bump();
  } else {
    (*this)(ConnectionStatus::DOWNLOADING);
    download_.begin();
  }
}

void OrderEntryCoinM::operator()(Trace<web::rest::Client::Disconnected> const &) {
  ++counter_.disconnect;
  (*this)(ConnectionStatus::DISCONNECTED);
  if (!download_.downloading()) {
    download_.reset();
  }
}

void OrderEntryCoinM::operator()(Trace<web::rest::Client::Latency> const &event) {
  auto &[trace_info, latency] = event;
  auto external_latency = ExternalLatency{
      .stream_id = stream_id_,
      .account = account_.name,
      .latency = latency.sample,
  };
  create_trace_and_dispatch(handler_, trace_info, external_latency);
  latency_.ping.update(latency.sample);
}

void OrderEntryCoinM::operator()(ConnectionStatus status) {
  if (utils::update(status_, status)) {
    TraceInfo trace_info;
    auto stream_status = StreamStatus{
        .stream_id = stream_id_,
        .account = account_.name,
        .supports = SUPPORTS,
        .transport = Transport::TCP,
        .protocol = Protocol::HTTP,
        .encoding = {Encoding::JSON},
        .priority = Priority::PRIMARY,
        .connection_status = status_,
        .interface = (*connection_).get_interface(),
        .authority = (*connection_).get_current_authority(),
        .path = (*connection_).get_current_path(),
        .proxy = (*connection_).get_proxy(),
    };
    log::info("stream_status={}"sv, stream_status);
    create_trace_and_dispatch(handler_, trace_info, stream_status);
  }
}

uint32_t OrderEntryCoinM::download(OrderEntryState state) {
  switch (state) {
    using enum OrderEntryState;
    case UNDEFINED:
      assert(false);
      break;
    case DONE:
      (*this)(ConnectionStatus::READY);
      return 0;
  }
  assert(false);
  return 0;
}

// orders-create

void OrderEntryCoinM::orders_create(
    Event<CreateOrder> const &event, server::oms::Order const &order, server::oms::RefData const &ref_data, std::string_view const &request_id) {
  profile_.orders_create([&]() {
    if (!ready()) {
      throw server::oms::NotReady{"not ready"sv};
    }
    auto &[message_info, create_order] = event;
    auto helper = [&](auto &security) {
      auto path = shared_.api.order_management.orders_create;
      auto query = json::Encoder::orders_create_coin_m(encode_buffer_, create_order, order, ref_data, request_id, security);
      auto headers = account_.create_headers(path, query, {}, request_id);
      auto request = web::rest::Request{
          .method = web::http::Method::PUT,
          .path = path,
          .query = query,
          .accept = web::http::Accept::APPLICATION_JSON,
          .content_type = web::http::ContentType::APPLICATION_JSON,
          .headers = headers,
          .body = {},
          .quality_of_service = {},
      };
      log::info<2>("DEBUG request={}"sv, request);
      auto callback = [this, user_id = message_info.source, order_id = create_order.order_id]([[maybe_unused]] auto &request_id, auto &response) {
        uint32_t version = 1;
        TraceInfo trace_info;
        Trace event{trace_info, response};
        orders_create_ack(event, user_id, order_id, version);
      };
      (*connection_)(request_id, request, callback);
    };
    if (shared_.find_security(create_order.symbol, helper)) {
    } else {
      log::warn("*** NO SECURITY INFO ***"sv);
      throw server::oms::NotReady{"not ready"sv};  // XXX FIXME TODO
    }
  });
}

void OrderEntryCoinM::orders_create_ack(Trace<web::rest::Response> const &event, uint8_t user_id, uint64_t order_id, uint32_t version) {
  profile_.orders_create_ack([&]() {
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(DEBUG origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
      auto response = server::oms::Response{
          .request_type = RequestType::CREATE_ORDER,
          .origin = origin,
          .request_status = status,
          .error = error,
          .text = text,
          .version = version,
          .request_id = {},
          .external_order_id = {},
          .quantity = NaN,
          .price = NaN,
      };
      Trace event_2{event, response};
      (*this)(event_2, user_id, order_id);
    };
    auto handle_success = [&](auto &body) {
      // log::warn(R"(DEBUG body="{}")"sv, body);
      json::OrdersCreateAck orders_create_ack{body, decode_buffer_};
      if (orders_create_ack.code == 0) {
        Trace event_2{event, orders_create_ack};
        (*this)(event_2, user_id, order_id, version);
      } else {
        handle_error(Origin::EXCHANGE, RequestStatus::REJECTED, json::guess_error(orders_create_ack.code), orders_create_ack.msg);
      }
    };
    process_response(event, handle_error, handle_success);
  });
}

void OrderEntryCoinM::operator()(Trace<json::OrdersCreateAck> const &event, uint8_t user_id, uint64_t order_id, uint32_t version) {
  auto &[trace_info, orders_create_ack] = event;
  log::info<2>("orders_create_ack={}"sv, orders_create_ack);
  auto &data = orders_create_ack.data;
  auto response = server::oms::Response{
      .request_type = RequestType::CREATE_ORDER,
      .origin = Origin::EXCHANGE,
      .request_status = RequestStatus::ACCEPTED,
      .error = {},
      .text = {},
      .version = version,
      .request_id = {},
      .external_order_id = {},
      .quantity = data.order_qty,
      .price = data.price,
  };
  auto order_update = server::oms::OrderUpdate{
      .account = account_.name,
      .exchange = shared_.settings.exchange,
      .symbol = data.symbol,
      .side = map(data.side),
      .position_effect = {},
      .margin_mode = {},
      .max_show_quantity = NaN,
      .order_type = map(data.order_type),
      .time_in_force = map(data.time_in_force),
      .execution_instructions = {},  // ???
      .create_time_utc = data.transact_time_ns,
      .update_time_utc = data.transact_time_ns,
      .external_account = {},
      .external_order_id = data.order_id,
      .client_order_id = data.cl_ord_id,
      .order_status = map(data.ord_status),
      .error = {},
      .text = orders_create_ack.msg,
      .quantity = data.order_qty,
      .price = data.price,
      .stop_price = data.stop_px,
      .leverage = NaN,
      .remaining_quantity = data.leaves_qty,
      .traded_quantity = data.cum_qty,
      .average_traded_price = NaN,
      .last_traded_quantity = NaN,
      .last_traded_price = NaN,
      .last_liquidity = {},
      .routing_id = {},
      .max_request_version = {},
      .max_response_version = {},
      .max_accepted_version = {},
      .update_type = UpdateType::INCREMENTAL,
      .sending_time_utc = orders_create_ack.request_time,  // ???
  };
  Trace event_2{trace_info, response};
  (*this)(event_2, user_id, order_id, order_update);
}

// orders-replace

void OrderEntryCoinM::orders_replace(
    Event<ModifyOrder> const &event,
    server::oms::Order const &order,
    server::oms::RefData const &ref_data,
    std::string_view const &request_id,
    [[maybe_unused]] std::string_view const &previous_request_id) {
  profile_.orders_replace([&]() {
    if (!ready()) {
      throw server::oms::NotReady{"not ready"sv};
    }
    auto &[message_info, modify_order] = event;
    auto path = shared_.api.order_management.orders_replace;
    auto query = json::Encoder::orders_replace_coin_m(encode_buffer_, modify_order, order, ref_data, request_id);
    auto headers = account_.create_headers(path, query, {}, request_id);
    auto request = web::rest::Request{
        .method = web::http::Method::PUT,
        .path = path,
        .query = query,
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = web::http::ContentType::APPLICATION_JSON,
        .headers = headers,
        .body = {},
        .quality_of_service = {},
    };
    log::info<2>("DEBUG request={}"sv, request);
    auto callback = [this, user_id = message_info.source, order_id = modify_order.order_id, version = modify_order.version](
                        [[maybe_unused]] auto &request_id, auto &response) {
      TraceInfo trace_info;
      Trace event{trace_info, response};
      orders_replace_ack(event, user_id, order_id, version);
    };
    (*connection_)(request_id, request, callback);
  });
}

void OrderEntryCoinM::orders_replace_ack(Trace<web::rest::Response> const &event, uint8_t user_id, uint64_t order_id, uint32_t version) {
  profile_.orders_replace_ack([&]() {
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(DEBUG origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
      auto response = server::oms::Response{
          .request_type = RequestType::MODIFY_ORDER,
          .origin = origin,
          .request_status = status,
          .error = error,
          .text = text,
          .version = version,
          .request_id = {},
          .external_order_id = {},
          .quantity = NaN,
          .price = NaN,
      };
      Trace event_2{event, response};
      (*this)(event_2, user_id, order_id);
    };
    auto handle_success = [&](auto &body) {
      json::OrdersReplaceAck orders_replace_ack{body, decode_buffer_};
      if (orders_replace_ack.code == 0) {
        Trace event_2{event, orders_replace_ack};
        (*this)(event_2, user_id, order_id, version);
      } else {
        handle_error(Origin::EXCHANGE, RequestStatus::REJECTED, json::guess_error(orders_replace_ack.code), orders_replace_ack.msg);
      }
    };
    process_response(event, handle_error, handle_success);
  });
}

void OrderEntryCoinM::operator()(
    Trace<json::OrdersReplaceAck> const &event, [[maybe_unused]] uint8_t user_id, [[maybe_unused]] uint64_t order_id, [[maybe_unused]] uint32_t version) {
  auto &[trace_info, orders_replace_ack] = event;
  log::info<2>("orders_replace_ack={}"sv, orders_replace_ack);
}

// orders-cancel

void OrderEntryCoinM::orders_cancel(
    Event<CancelOrder> const &event,
    server::oms::Order const &order,
    server::oms::RefData const &ref_data,
    std::string_view const &request_id,
    [[maybe_unused]] std::string_view const &previous_request_id) {
  profile_.orders_cancel([&]() {
    if (!ready()) {
      throw server::oms::NotReady{"not ready"sv};
    }
    auto &[message_info, cancel_order] = event;
    auto path = shared_.api.order_management.orders_cancel;
    auto query = json::Encoder::orders_cancel_coin_m(encode_buffer_, cancel_order, order, ref_data, request_id);
    auto headers = account_.create_headers(path, query, {}, request_id);
    auto request = web::rest::Request{
        .method = web::http::Method::DELETE,
        .path = path,
        .query = query,
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = web::http::ContentType::APPLICATION_JSON,
        .headers = headers,
        .body = {},
        .quality_of_service = {},
    };
    log::info<2>("DEBUG request={}"sv, request);
    auto callback = [this, user_id = message_info.source, order_id = cancel_order.order_id, version = cancel_order.version](
                        [[maybe_unused]] auto &request_id, auto &response) {
      TraceInfo trace_info;
      Trace event{trace_info, response};
      orders_cancel_ack(event, user_id, order_id, version);
    };
    (*connection_)(request_id, request, callback);
  });
}

void OrderEntryCoinM::orders_cancel_ack(Trace<web::rest::Response> const &event, uint8_t user_id, uint64_t order_id, uint32_t version) {
  profile_.orders_cancel_ack([&]() {
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(DEBUG origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
      auto response = server::oms::Response{
          .request_type = RequestType::CANCEL_ORDER,
          .origin = origin,
          .request_status = status,
          .error = error,
          .text = text,
          .version = version,
          .request_id = {},
          .external_order_id = {},
          .quantity = NaN,
          .price = NaN,
      };
      Trace event_2{event, response};
      (*this)(event_2, user_id, order_id);
    };
    auto handle_success = [&](auto &body) {
      json::OrdersCancelAck orders_cancel_ack{body, decode_buffer_};
      if (orders_cancel_ack.code == 0) {
        Trace event_2{event, orders_cancel_ack};
        (*this)(event_2, user_id, order_id, version);
      } else {
        handle_error(Origin::EXCHANGE, RequestStatus::REJECTED, json::guess_error(orders_cancel_ack.code), orders_cancel_ack.msg);
      }
    };
    process_response(event, handle_error, handle_success);
  });
}

void OrderEntryCoinM::operator()(Trace<json::OrdersCancelAck> const &event, uint8_t user_id, uint64_t order_id, uint32_t version) {
  auto &[trace_info, orders_cancel_ack] = event;
  log::info<2>("orders_cancel_ack={}"sv, orders_cancel_ack);
  auto &data = orders_cancel_ack.data;
  auto response = server::oms::Response{
      .request_type = RequestType::CANCEL_ORDER,
      .origin = Origin::EXCHANGE,
      .request_status = RequestStatus::ACCEPTED,
      .error = {},
      .text = {},
      .version = version,
      .request_id = {},
      .external_order_id = {},
      .quantity = data.order_qty,
      .price = data.price,
  };
  auto order_update = server::oms::OrderUpdate{
      .account = account_.name,
      .exchange = shared_.settings.exchange,
      .symbol = data.symbol,
      .side = map(data.side),
      .position_effect = {},
      .margin_mode = {},
      .max_show_quantity = NaN,
      .order_type = map(data.order_type),
      .time_in_force = map(data.time_in_force),
      .execution_instructions = {},  // ???
      .create_time_utc = {},
      .update_time_utc = data.transact_time_ns,
      .external_account = {},
      .external_order_id = data.order_id,
      .client_order_id = data.cl_ord_id,
      .order_status = map(data.ord_status),
      .error = {},
      .text = orders_cancel_ack.msg,
      .quantity = data.order_qty,
      .price = data.price,
      .stop_price = data.stop_px,
      .leverage = NaN,
      .remaining_quantity = data.leaves_qty,
      .traded_quantity = data.cum_qty,
      .average_traded_price = NaN,
      .last_traded_quantity = NaN,
      .last_traded_price = NaN,
      .last_liquidity = {},
      .routing_id = {},
      .max_request_version = {},
      .max_response_version = {},
      .max_accepted_version = {},
      .update_type = UpdateType::INCREMENTAL,
      .sending_time_utc = orders_cancel_ack.request_time,
  };
  Trace event_2{trace_info, response};
  (*this)(event_2, user_id, order_id, order_update);
}

// orders-all

void OrderEntryCoinM::orders_all(Event<CancelAllOrders> const &event, std::string_view const &request_id) {
  profile_.orders_all([&]() {
    if (!ready()) {
      throw server::oms::NotReady{"not ready"sv};
    }
    auto &[message_info, cancel_all_orders] = event;
    auto path = shared_.api.order_management.orders_all;
    auto helper = [&](auto &symbol) {
      auto query = json::Encoder::orders_all(encode_buffer_, cancel_all_orders, symbol, request_id);
      auto headers = account_.create_headers(path, query, {}, request_id);
      auto request = web::rest::Request{
          .method = web::http::Method::DELETE,
          .path = path,
          .query = query,
          .accept = web::http::Accept::APPLICATION_JSON,
          .content_type = web::http::ContentType::APPLICATION_JSON,
          .headers = headers,
          .body = {},
          .quality_of_service = {},
      };
      log::info<2>("DEBUG request={}"sv, request);
      auto callback = [this, user_id = message_info.source]([[maybe_unused]] auto &request_id, auto &response) {
        TraceInfo trace_info;
        Trace event{trace_info, response};
        orders_all_ack(event, user_id);
      };
      (*connection_)(request_id, request, callback);
    };
    if (shared_.dispatcher.get_all_order_symbols(helper, account_.name)) {
    } else {
      log::warn("*** NOT POSSIBLE TO CANCEL ALL OPEN ORDERS (NO SYMBOLS) ***"sv);
    }
  });
}

void OrderEntryCoinM::orders_all_ack(Trace<web::rest::Response> const &event, [[maybe_unused]] uint8_t user_id) {
  profile_.orders_all_ack([&]() {
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(DEBUG origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
      auto orders_all_ack = CancelAllOrdersAck{
          .stream_id = stream_id_,
          .account = account_.name,
          .order_id = {},
          .exchange = shared_.settings.exchange,
          .symbol = {},
          .side = {},
          .origin = Origin::EXCHANGE,
          .request_status = RequestStatus::REJECTED,
          .error = error,
          .text = text,
          .request_id = {},  // ???
          .external_account = {},
          .number_of_affected_orders = {},
          .round_trip_latency = {},
          .user = {},
          .strategy_id = {},
      };
      TraceInfo trace_info;
      Trace event_2{trace_info, orders_all_ack};
      shared_(event_2);
    };
    auto handle_success = [&](auto &body) {
      json::OrdersAllAck orders_all_ack{body, decode_buffer_};
      if (orders_all_ack.code == 0) {
        Trace event_2{event, orders_all_ack};
        (*this)(event_2, user_id);
      } else {
        handle_error(Origin::EXCHANGE, RequestStatus::REJECTED, json::guess_error(orders_all_ack.code), orders_all_ack.msg);
      }
    };
    process_response(event, handle_error, handle_success);
  });
}

void OrderEntryCoinM::operator()(Trace<json::OrdersAllAck> const &event, [[maybe_unused]] uint8_t user_id) {
  auto &[trace_info, orders_all_ack] = event;
  log::info<2>("orders_all_ack={}"sv, orders_all_ack);
}

// helpers

void OrderEntryCoinM::process_response(web::rest::Response const &response, auto error_handler, auto success_handler) {
  try {
    auto [status, category, body] = response.result();
    switch (category) {
      using enum web::http::Category;
      case UNKNOWN:
      case INFORMATIONAL_RESPONSE:
        response.expect(web::http::Status::OK);  // throws
        break;
      case SUCCESS:
        success_handler(body);
        break;
      case REDIRECTION:
        log::fatal("Unexpected: URL is being redirected"sv);
      case CLIENT_ERROR:
        switch (status) {
          using enum web::http::Status;
          case FORBIDDEN:            // 403
          case I_AM_A_TEAPOT:        // 418
          case TOO_MANY_REQUESTS: {  // 429
            auto message = fmt::format("{}"sv, status);
            error_handler(Origin::EXCHANGE, RequestStatus::REJECTED, Error::REQUEST_RATE_LIMIT_REACHED, message);
            break;
          }
          case CONFLICT:  // 409
            assert(false);
            [[fallthrough]];
          default: {
            // json::Message error{body};
            // XXX HANS error_handler(Origin::EXCHANGE, RequestStatus::REJECTED, json::guess_error(error.code), error.msg);
          }
        }
        break;
      case SERVER_ERROR: {
        auto message = fmt::format("{}"sv, status);
        error_handler(Origin::EXCHANGE, RequestStatus::REJECTED, Error::UNKNOWN, message);
        break;
      }
    }
  } catch (server::oms::Exception &e) {
    log::warn(R"(Exception type={}, what="{}")"sv, typeid(e).name(), e.what());
    error_handler(e.origin, e.status, e.error, e.what());
  } catch (NetworkError &e) {
    log::warn(R"(Exception type={}, what="{}")"sv, typeid(e).name(), e.what());
    error_handler(Origin::GATEWAY, e.request_status(), e.error(), e.what());
  } catch (std::exception &e) {
    log::warn(R"(Exception type={}, what="{}")"sv, typeid(e).name(), e.what());
    error_handler(Origin::EXCHANGE, RequestStatus::ERROR, Error::UNKNOWN, e.what());
  }
}

template <typename... Args>
void OrderEntryCoinM::operator()(Trace<server::oms::Response> const &event, uint8_t user_id, uint64_t order_id, Args &&...args) {
  auto &[trace_info, response] = event;
  if (shared_.update_order(user_id, order_id, stream_id_, trace_info, response, std::forward<Args>(args)..., []([[maybe_unused]] auto &order) {})) {
  } else {
    log::warn("Did not find order: user_id={}, order_id={}"sv, user_id, order_id);
  }
}

void OrderEntryCoinM::operator()(Trace<server::oms::OrderUpdate> const &event, std::string_view const &client_order_id) {
  auto &[trace_info, order_update] = event;
  if (shared_.update_order(client_order_id, stream_id_, trace_info, order_update, [&]([[maybe_unused]] auto &order) {})) {
  } else {
    log::warn("*** EXTERNAL ORDER ***"sv);
  }
}

}  // namespace phemex_futures
}  // namespace roq
