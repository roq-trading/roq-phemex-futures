/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/phemex_futures/gateway/order_entry_coin_m.hpp"

#include "roq/mask.hpp"

#include "roq/utils/metrics/factory.hpp"

#include "roq/server/oms/exceptions.hpp"

#include "roq/phemex_futures/protocol/json/encoder.hpp"
#include "roq/phemex_futures/protocol/json/map.hpp"
#include "roq/phemex_futures/protocol/json/utils.hpp"

using namespace std::literals;

namespace roq {
namespace phemex_futures {
namespace gateway {

// === CONSTANTS ===

namespace {
auto const NAME = "om"sv;

auto const SUPPORTS = Mask{
    SupportType::CREATE_ORDER,
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

auto create_connection(auto &handler, auto &settings, auto &context, auto &shared) {
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
  return web::rest::Client::create(handler, context, config, shared.throttle);
}

struct create_metrics final : public utils::metrics::Factory {
  create_metrics(auto &settings, auto &group, auto const &function) : utils::metrics::Factory{settings.app.name, group, function} {}
};
}  // namespace

// === IMPLEMENTATION ===

OrderEntryCoinM::OrderEntryCoinM(Handler &handler, io::Context &context, uint16_t stream_id, Account &account, Shared &shared, Request &request, bool master)
    : server::OrderActionStream{stream_id, account.name, SUPPORTS}, handler_{handler}, name_{create_name(stream_id, account)}, master_{master},
      connection_{create_connection(*this, shared.settings, context, shared)}, decode_buffer_{shared.settings.misc.decode_buffer_size, MAX_DECODE_BUFFER_DEPTH},
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
          .order = create_metrics(shared.settings, name_, "order"sv),
          .order_ack = create_metrics(shared.settings, name_, "order_ack"sv),
          .user_fills = create_metrics(shared.settings, name_, "user_fills"sv),
          .user_fills_ack = create_metrics(shared.settings, name_, "user_fills_ack"sv),
      },
      latency_{
          .ping = create_metrics(shared.settings, name_, "ping"sv),
      },
      account_{account}, shared_{shared}, request_{request} {
}

// server::Stream

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
      .write(profile_.order, metrics::Type::PROFILE)
      .write(profile_.order_ack, metrics::Type::PROFILE)
      .write(profile_.user_fills, metrics::Type::PROFILE)
      .write(profile_.user_fills_ack, metrics::Type::PROFILE)
      // latency
      .write(latency_.ping, metrics::Type::LATENCY);
}

void OrderEntryCoinM::operator()(Trace<Start> const &event) {
  (*connection_)(event);
}

void OrderEntryCoinM::operator()(Trace<Stop> const &event) {
  (*connection_)(event);
}

void OrderEntryCoinM::operator()(Trace<Timer> const &event) {
  auto &[trace_info, timer] = event;
  if ((*connection_)(event)) {
    if (ready()) {
      if (master_) {
        check_request_queue(trace_info);
      }
    }
  }
}

void OrderEntryCoinM::operator()(Trace<ConnectionStatus> const &event, std::string_view const &reason) {
  auto &[trace_info, connection_status] = event;
  if (update(connection_status, reason)) {
    auto stream_status = StreamStatus{
        .stream_id = stream_id,
        .account = account,
        .supports = supports,
        .transport = Transport::TCP,
        .protocol = Protocol::HTTP,
        .encoding = {Encoding::JSON},
        .priority = Priority::PRIMARY,
        .connection_status = connection_status,
        .reason = reason,
        .interface = (*connection_).get_interface(),
        .proxy = (*connection_).get_proxy(),
        .peer = (*connection_).get_current_peer(),
        .authority = (*connection_).get_current_authority(),
        .path = (*connection_).get_current_path(),
    };
    log::info("stream_status={}"sv, stream_status);
    create_trace_and_dispatch(shared_.dispatcher, trace_info, stream_status);
  }
}

// server::OrderActionStream

void OrderEntryCoinM::operator()(Trace<server::oms::OrderActionBatchBegin> const &) {
  throw server::oms::NotSupported{"not supported"sv};
}

void OrderEntryCoinM::operator()(Trace<server::oms::OrderActionBatchCommit> const &) {
  throw server::oms::NotSupported{"not supported"sv};
}

void OrderEntryCoinM::operator()(Trace<server::oms::OrderActionBatchRollback> const &) {
  throw server::oms::NotSupported{"not supported"sv};
}

void OrderEntryCoinM::operator()(
    Trace<CreateOrder> const &event, server::oms::Order const &order, server::oms::RefData const &ref_data, std::string_view const &request_id) {
  orders_create(event, order, ref_data, request_id);
}

void OrderEntryCoinM::operator()(
    Trace<ModifyOrder> const &,
    server::oms::Order const &,
    server::oms::RefData const &,
    [[maybe_unused]] std::string_view const &request_id,
    [[maybe_unused]] std::string_view const &previous_request_id) {
  throw server::oms::NotSupported{"not supported"sv};
}

void OrderEntryCoinM::operator()(
    Trace<CancelOrder> const &event,
    server::oms::Order const &order,
    server::oms::RefData const &ref_data,
    std::string_view const &request_id,
    std::string_view const &previous_request_id) {
  orders_cancel(event, order, ref_data, request_id, previous_request_id);
}

void OrderEntryCoinM::operator()(Trace<CancelAllOrders> const &event, std::string_view const &request_id) {
  orders_all(event, request_id);
}

// web::rest::Client::Handler

void OrderEntryCoinM::operator()(Trace<web::rest::Connected> const &event) {
  auto &[trace_info, connected] = event;
  publish(trace_info, ConnectionStatus::READY);
}

void OrderEntryCoinM::operator()(Trace<web::rest::Disconnected> const &event) {
  auto &[trace_info, disconnected] = event;
  ++counter_.disconnect;
  publish(trace_info, ConnectionStatus::DISCONNECTED);
}

void OrderEntryCoinM::operator()(Trace<web::rest::Latency> const &event) {
  auto &[trace_info, latency] = event;
  auto external_latency = ExternalLatency{
      .stream_id = stream_id,
      .account = account_.name,
      .latency = latency.sample,
  };
  create_trace_and_dispatch(shared_.dispatcher, trace_info, external_latency);
  latency_.ping.update(latency.sample);
}

// orders-create

void OrderEntryCoinM::orders_create(
    Trace<CreateOrder> const &event, server::oms::Order const &order, server::oms::RefData const &ref_data, std::string_view const &request_id) {
  profile_.orders_create([&]() {
    if (!ready()) {
      throw server::oms::NotReady{"not ready"sv};
    }
    auto &[trace_info, create_order] = event;
    auto helper = [&](auto &security) {
      auto path = shared_.api.order_management.orders_create;
      auto query = protocol::json::Encoder::orders_create_coin_m(encode_buffer_, create_order, order, ref_data, request_id, security);
      auto headers = account_.create_headers(trace_info, path, query, {}, request_id);
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
      auto callback = [this, user_id = order.user_id, order_id = order.order_id](auto &event, [[maybe_unused]] auto &request_id) {
        uint32_t version = 1;
        orders_create_ack(event, user_id, order_id, version);
      };
      (*connection_)(request, callback, request_id);
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
    auto &[trace_info, response] = event;
    log::warn("DEBUG response={}"sv, response);
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
          .external_trace_id = {},
          .external_order_id = {},
          .client_order_id = {},
          .quantity = NaN,
          .price = NaN,
      };
      create_trace_and_dispatch(shared_.dispatcher, trace_info, response, stream_id, user_id, order_id);
    };
    auto handle_success = [&](auto &body) {
      // log::warn(R"(DEBUG body="{}")"sv, body);
      protocol::json::OrdersCreateAck orders_create_ack{body, decode_buffer_};
      log::info<2>("orders_create_ack={}"sv, orders_create_ack);
      if (orders_create_ack.code != 0) {
        auto error = map(orders_create_ack.code).template get<Error>();
        handle_error(Origin::EXCHANGE, RequestStatus::REJECTED, error, orders_create_ack.msg);
      } else if (shared_.settings.oms.allow_unsafe_order_ack) {
        create_trace_and_dispatch_2(trace_info, orders_create_ack, user_id, order_id, version);
      }
    };
    process_response(event, handle_error, handle_success);
  });
}

void OrderEntryCoinM::operator()(Trace<protocol::json::OrdersCreateAck> const &event, uint8_t user_id, uint64_t order_id, uint32_t version) {
  auto &[trace_info, orders_create_ack] = event;
  auto &data = orders_create_ack.data;
  auto response = server::oms::Response{
      .request_type = RequestType::CREATE_ORDER,
      .origin = Origin::EXCHANGE,
      .request_status = RequestStatus::ACCEPTED,
      .error = {},
      .text = {},
      .version = version,
      .request_id = {},
      .external_trace_id = {},
      .external_order_id = data.order_id,
      .client_order_id = data.cl_ord_id,
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
      .execution_destination = {},
      .create_time_utc = data.transact_time_ns,
      .update_time_utc = data.transact_time_ns,
      .external_trace_id = {},
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
  create_trace_and_dispatch(shared_.dispatcher, trace_info, response, order_update, stream_id, user_id, order_id);
}

// orders-replace

void OrderEntryCoinM::orders_replace(
    Trace<ModifyOrder> const &event,
    server::oms::Order const &order,
    server::oms::RefData const &ref_data,
    std::string_view const &request_id,
    [[maybe_unused]] std::string_view const &previous_request_id) {
  profile_.orders_replace([&]() {
    if (!ready()) {
      throw server::oms::NotReady{"not ready"sv};
    }
    auto &[trace_info, modify_order] = event;
    auto path = shared_.api.order_management.orders_replace;
    auto query = protocol::json::Encoder::orders_replace_coin_m(encode_buffer_, modify_order, order, ref_data, request_id);
    auto headers = account_.create_headers(trace_info, path, query, {}, request_id);
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
    auto callback = [this, user_id = order.user_id, order_id = order.order_id, version = modify_order.version](auto &event, [[maybe_unused]] auto &request_id) {
      orders_replace_ack(event, user_id, order_id, version);
    };
    (*connection_)(request, callback, request_id);
  });
}

void OrderEntryCoinM::orders_replace_ack(Trace<web::rest::Response> const &event, uint8_t user_id, uint64_t order_id, uint32_t version) {
  profile_.orders_replace_ack([&]() {
    auto &[trace_info, response] = event;
    log::warn("DEBUG response={}"sv, response);
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
          .external_trace_id = {},
          .external_order_id = {},
          .client_order_id = {},
          .quantity = NaN,
          .price = NaN,
      };
      create_trace_and_dispatch(shared_.dispatcher, trace_info, response, stream_id, user_id, order_id);
    };
    auto handle_success = [&](auto &body) {
      protocol::json::OrdersReplaceAck orders_replace_ack{body, decode_buffer_};
      log::info<2>("orders_replace_ack={}"sv, orders_replace_ack);
      if (orders_replace_ack.code != 0) {
        auto error = map(orders_replace_ack.code).template get<Error>();
        handle_error(Origin::EXCHANGE, RequestStatus::REJECTED, error, orders_replace_ack.msg);
      } else if (shared_.settings.oms.allow_unsafe_order_ack) {
        create_trace_and_dispatch_2(trace_info, orders_replace_ack, user_id, order_id, version);
      }
    };
    process_response(event, handle_error, handle_success);
  });
}

void OrderEntryCoinM::operator()(
    Trace<protocol::json::OrdersReplaceAck> const &event,
    [[maybe_unused]] uint8_t user_id,
    [[maybe_unused]] uint64_t order_id,
    [[maybe_unused]] uint32_t version) {
  [[maybe_unused]] auto &[trace_info, orders_replace_ack] = event;
}

// orders-cancel

void OrderEntryCoinM::orders_cancel(
    Trace<CancelOrder> const &event,
    server::oms::Order const &order,
    server::oms::RefData const &ref_data,
    std::string_view const &request_id,
    [[maybe_unused]] std::string_view const &previous_request_id) {
  profile_.orders_cancel([&]() {
    if (!ready()) {
      throw server::oms::NotReady{"not ready"sv};
    }
    auto &[trace_info, cancel_order] = event;
    auto path = shared_.api.order_management.orders_cancel;
    auto query = protocol::json::Encoder::orders_cancel_coin_m(encode_buffer_, cancel_order, order, ref_data, request_id);
    auto headers = account_.create_headers(trace_info, path, query, {}, request_id);
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
    auto callback = [this, user_id = order.user_id, order_id = order.order_id, version = cancel_order.version](auto &event, [[maybe_unused]] auto &request_id) {
      orders_cancel_ack(event, user_id, order_id, version);
    };
    (*connection_)(request, callback, request_id);
  });
}

void OrderEntryCoinM::orders_cancel_ack(Trace<web::rest::Response> const &event, uint8_t user_id, uint64_t order_id, uint32_t version) {
  profile_.orders_cancel_ack([&]() {
    auto &[trace_info, response] = event;
    log::warn("DEBUG response={}"sv, response);
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
          .external_trace_id = {},
          .external_order_id = {},
          .client_order_id = {},
          .quantity = NaN,
          .price = NaN,
      };
      create_trace_and_dispatch(shared_.dispatcher, trace_info, response, stream_id, user_id, order_id);
    };
    auto handle_success = [&](auto &body) {
      protocol::json::OrdersCancelAck orders_cancel_ack{body, decode_buffer_};
      log::info<2>("orders_cancel_ack={}"sv, orders_cancel_ack);
      if (orders_cancel_ack.code != 0) {
        auto error = map(orders_cancel_ack.code).template get<Error>();
        handle_error(Origin::EXCHANGE, RequestStatus::REJECTED, error, orders_cancel_ack.msg);
      } else if (shared_.settings.oms.allow_unsafe_order_ack) {
        create_trace_and_dispatch_2(trace_info, orders_cancel_ack, user_id, order_id, version);
      }
    };
    process_response(event, handle_error, handle_success);
  });
}

void OrderEntryCoinM::operator()(Trace<protocol::json::OrdersCancelAck> const &event, uint8_t user_id, uint64_t order_id, uint32_t version) {
  auto &[trace_info, orders_cancel_ack] = event;
  auto &data = orders_cancel_ack.data;
  auto response = server::oms::Response{
      .request_type = RequestType::CANCEL_ORDER,
      .origin = Origin::EXCHANGE,
      .request_status = RequestStatus::ACCEPTED,
      .error = {},
      .text = {},
      .version = version,
      .request_id = {},
      .external_trace_id = {},
      .external_order_id = data.order_id,
      .client_order_id = data.cl_ord_id,
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
      .execution_destination = {},
      .create_time_utc = {},
      .update_time_utc = data.transact_time_ns,
      .external_trace_id = {},
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
  create_trace_and_dispatch(shared_.dispatcher, trace_info, response, order_update, stream_id, user_id, order_id);
}

// orders-all

void OrderEntryCoinM::orders_all(Trace<CancelAllOrders> const &event, std::string_view const &request_id) {
  profile_.orders_all([&]() {
    if (!ready()) {
      throw server::oms::NotReady{"not ready"sv};
    }
    auto &[trace_info, cancel_all_orders] = event;
    auto path = shared_.api.order_management.orders_all;
    auto helper = [&](auto &symbol) {
      auto query = protocol::json::Encoder::orders_all(encode_buffer_, cancel_all_orders, symbol, request_id);
      auto headers = account_.create_headers(trace_info, path, query, {}, request_id);
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
      auto callback = [this](auto &event, [[maybe_unused]] auto &request_id) { orders_all_ack(event); };
      (*connection_)(request, callback, request_id);
    };
    if (shared_.dispatcher.get_all_order_symbols(helper, account_.name)) {
    } else {
      log::warn("*** NOT POSSIBLE TO CANCEL ALL OPEN ORDERS (NO SYMBOLS) ***"sv);
    }
  });
}

void OrderEntryCoinM::orders_all_ack(Trace<web::rest::Response> const &event) {
  profile_.orders_all_ack([&]() {
    auto &[trace_info, response] = event;
    log::warn("DEBUG response={}"sv, response);
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(DEBUG origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
      auto cancel_all_orders_ack = CancelAllOrdersAck{
          .stream_id = stream_id,
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
      create_trace_and_dispatch(shared_.dispatcher, trace_info, cancel_all_orders_ack);
    };
    auto handle_success = [&](auto &body) {
      protocol::json::OrdersAllAck orders_all_ack{body, decode_buffer_};
      log::info<2>("orders_all_ack={}"sv, orders_all_ack);
      if (orders_all_ack.code != 0) {
        auto error = map(orders_all_ack.code).template get<Error>();
        handle_error(Origin::EXCHANGE, RequestStatus::REJECTED, error, orders_all_ack.msg);
      } else if (shared_.settings.oms.allow_unsafe_order_ack) {
        create_trace_and_dispatch_2(trace_info, orders_all_ack);
      }
    };
    process_response(event, handle_error, handle_success);
  });
}

void OrderEntryCoinM::operator()(Trace<protocol::json::OrdersAllAck> const &event) {
  [[maybe_unused]] auto &[trace_info, orders_all_ack] = event;
}

// order

void OrderEntryCoinM::get_order(
    TraceInfo const &trace_info, std::string_view const &client_order_id, std::string_view const &external_order_id, std::string_view const &symbol) {
  profile_.order([&]() {
    if (!ready()) {
      throw server::oms::NotReady{"not ready"sv};
    }
    auto path = shared_.api.order_management.order;
    auto query = protocol::json::Encoder::order_coin_m(encode_buffer_, client_order_id, external_order_id, symbol);
    auto headers = account_.create_headers(trace_info, path, query, {});
    auto request = web::rest::Request{
        .method = web::http::Method::GET,
        .path = path,
        .query = query,
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = web::http::ContentType::APPLICATION_X_WWW_FORM_URLENCODED,
        .headers = headers,
        .body = {},
        .quality_of_service = {},
    };
    log::info<2>("DEBUG request={}"sv, request);
    auto callback = [this, client_order_id = std::string{client_order_id}](auto &event, [[maybe_unused]] auto &request_id) {
      get_order_ack(event, client_order_id);
    };
    (*connection_)(request, callback, "order"sv);
  });
}

void OrderEntryCoinM::get_order_ack(Trace<web::rest::Response> const &event, [[maybe_unused]] std::string_view const &client_order_id) {
  profile_.order_ack([&]() {
    auto &[trace_info, response] = event;
    log::warn("DEBUG response={}"sv, response);
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(DEBUG origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
    };
    auto handle_success = [&](auto &body) {
      log::warn("DEBUG {}"sv, body);
      protocol::json::GetOrderAck get_order_ack{body, decode_buffer_};
      log::info<2>("get_order_ack={}"sv, get_order_ack);
      create_trace_and_dispatch_2(trace_info, get_order_ack);
    };
    process_response(event, handle_error, handle_success);
    //
    if (download_stale_orders_counter_) {
      --download_stale_orders_counter_;
    } else {
      log::fatal("Unexpected"sv);
    }
    log::warn("DEBUG remaining orders: {}"sv, download_stale_orders_counter_);
    if (download_stale_orders_counter_ == 0) {
      request_.stale_orders.commit(trace_info);
    }
  });
}

void OrderEntryCoinM::operator()(Trace<protocol::json::GetOrderAck> const &event) {
  auto &[trace_info, get_order_ack] = event;
  for (auto &item : get_order_ack.data) {
    auto order_update = server::oms::OrderUpdate{
        .account = account_.name,
        .exchange = shared_.settings.exchange,
        .symbol = item.symbol,
        .side = map(item.side),
        .position_effect = {},
        .margin_mode = {},
        .max_show_quantity = NaN,
        .order_type = map(item.order_type),
        .time_in_force = map(item.time_in_force),
        .execution_instructions = {},  // ???
        .execution_destination = {},
        .create_time_utc = item.transact_time_ns,
        .update_time_utc = item.transact_time_ns,
        .external_trace_id = {},
        .external_account = {},
        .external_order_id = item.order_id,
        .client_order_id = item.cl_ord_id,
        .order_status = map(item.ord_status),
        .error = {},
        .text = get_order_ack.msg,
        .quantity = item.order_qty,
        .price = item.price,
        .stop_price = NaN,
        .leverage = NaN,
        .remaining_quantity = item.leaves_qty,
        .traded_quantity = item.cum_qty,
        .average_traded_price = NaN,
        .last_traded_quantity = NaN,
        .last_traded_price = NaN,
        .last_liquidity = {},
        .routing_id = {},
        .max_request_version = {},
        .max_response_version = {},
        .max_accepted_version = {},
        .update_type = UpdateType::INCREMENTAL,
        .sending_time_utc = item.action_time_ns,  // ???
    };
    create_trace_and_dispatch(shared_.dispatcher, trace_info, order_update, stream_id);
  }
}

// user-fills

void OrderEntryCoinM::get_user_fills(TraceInfo const &trace_info) {
  profile_.user_fills([&]() {
    if (!ready()) {
      throw server::oms::NotReady{"not ready"sv};
    }
    auto path = shared_.api.order_management.user_fills;
    auto query = protocol::json::Encoder::user_fills_coin_m(encode_buffer_);
    auto headers = account_.create_headers(trace_info, path, query, {});
    auto request = web::rest::Request{
        .method = web::http::Method::GET,
        .path = path,
        .query = query,
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = web::http::ContentType::APPLICATION_X_WWW_FORM_URLENCODED,
        .headers = headers,
        .body = {},
        .quality_of_service = {},
    };
    log::info<2>("DEBUG request={}"sv, request);
    auto callback = [this](auto &event, [[maybe_unused]] auto &request_id) { get_user_fills_ack(event); };
    (*connection_)(request, callback, "user-fills"sv);
  });
}

void OrderEntryCoinM::get_user_fills_ack(Trace<web::rest::Response> const &event) {
  profile_.user_fills_ack([&]() {
    auto &[trace_info, response] = event;
    log::warn("DEBUG response={}"sv, response);
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(DEBUG origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
    };
    auto handle_success = [&](auto &body) { log::warn("DEBUG {}"sv, body); };
    process_response(event, handle_error, handle_success);
    request_.trades.commit(trace_info);
  });
}

// helpers

void OrderEntryCoinM::check_request_queue(TraceInfo const &trace_info) {
  request_.stale_orders.check_pending(trace_info, [this](auto &trace_info) {
    // XXX FIXME TODO throttle requests
    auto helper = [&](auto &order) { get_order(trace_info, order.client_order_id, order.external_order_id, order.symbol); };
    for (auto &[user_id, order_id] : request_.details_stale_orders) {
      if (shared_.dispatcher.find_order(user_id, order_id, helper)) {
        ++download_stale_orders_counter_;
      } else {
        log::warn("Unexpected: user_id={}, order_id={}"sv, user_id, order_id);
      }
    }
    request_.details_stale_orders.clear();
  });
  request_.trades.check_pending(trace_info, [this](auto &trace_info) { get_user_fills(trace_info); });
}

void OrderEntryCoinM::process_response(Trace<web::rest::Response> const &event, auto error_handler, auto success_handler) {
  auto &[trace, response] = event;
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
          case TOO_MANY_REQUESTS: {  // 429
            auto message = fmt::format("{}"sv, status);
            error_handler(Origin::EXCHANGE, RequestStatus::REJECTED, Error::REQUEST_RATE_LIMIT_REACHED, message);
            break;
          }
          case CONFLICT:  // 409
            assert(false);
            [[fallthrough]];
          default:
            error_handler(Origin::EXCHANGE, RequestStatus::REJECTED, Error::UNKNOWN, ""sv);
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

}  // namespace gateway
}  // namespace phemex_futures
}  // namespace roq
