/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/phemex_futures/drop_copy_usd_m.hpp"

#include "roq/mask.hpp"

#include "roq/utils/common.hpp"
#include "roq/utils/safe_cast.hpp"
#include "roq/utils/update.hpp"

#include "roq/utils/exceptions/unhandled.hpp"

#include "roq/utils/metrics/factory.hpp"

#include "roq/phemex_futures/json/map.hpp"
#include "roq/phemex_futures/json/utils.hpp"

using namespace std::literals;

namespace roq {
namespace phemex_futures {

// === CONSTANTS ===

namespace {
auto const NAME = "ex"sv;

auto const SUPPORTS = Mask{
    SupportType::ORDER,
    SupportType::TRADE,
    SupportType::FUNDS,
    SupportType::POSITION,
};

size_t const MAX_DECODE_BUFFER_DEPTH = 2;

uint64_t const REQUEST_ID_AUTH = 1;
uint64_t const REQUEST_ID_AOP = 2;
}  // namespace

// === HELPERS ===

namespace {
auto create_name(auto stream_id, auto &account) {
  return fmt::format("{}:{}:{}"sv, stream_id, NAME, account.name);
}

auto create_connection(auto &handler, auto &settings, auto &context) {
  auto uri = settings.ws.uri;
  auto config = web::socket::Client::Config{
      // connection
      .interface = {},
      .uris = {&uri, 1},
      .host = {},
      .validate_certificate = settings.net.tls_validate_certificate,
      // connection manager
      .connection_timeout = settings.net.connection_timeout,
      .disconnect_on_idle_timeout = {},
      .always_reconnect = true,
      // proxy
      .proxy = {},
      // http
      .user_agent = ROQ_PACKAGE_NAME,
      .request_timeout = {},
      .ping_frequency = settings.ws.ping_freq,
      // implementation
      .decode_buffer_size = settings.misc.decode_buffer_size,
      .encode_buffer_size = settings.misc.encode_buffer_size,
  };
  return web::socket::Client::create(handler, context, config, []() { return std::string(); });
}

struct create_metrics final : public utils::metrics::Factory {
  create_metrics(auto &settings, auto &group, auto const &function) : utils::metrics::Factory{settings.app.name, group, function} {}
};
}  // namespace

// === IMPLEMENTATION ===

DropCopyUsdM::DropCopyUsdM(DropCopy::Handler &handler, io::Context &context, uint16_t stream_id, Account &account, Shared &shared)
    : handler_{handler}, stream_id_{stream_id}, name_{create_name(stream_id_, account)}, connection_{create_connection(*this, shared.settings, context)},
      decode_buffer_{shared.settings.misc.decode_buffer_size, MAX_DECODE_BUFFER_DEPTH},
      counter_{
          .disconnect = create_metrics(shared.settings, name_, "disconnect"sv),
      },
      profile_{
          .parse = create_metrics(shared.settings, name_, "parse"sv),
      },
      latency_{
          .ping = create_metrics(shared.settings, name_, "ping"sv),
          .heartbeat = create_metrics(shared.settings, name_, "heartbeat"sv),
      },
      account_{account}, shared_{shared} {
}

bool DropCopyUsdM::ready() const {
  return (*connection_).ready();
}

void DropCopyUsdM::operator()(Event<Start> const &) {
  (*connection_).start();
}

void DropCopyUsdM::operator()(Event<Stop> const &) {
  (*connection_).stop();
}

void DropCopyUsdM::operator()(Event<Timer> const &event) {
  auto now = event.value.now;
  (*connection_).refresh(now);
  if (ready()) {
    if (next_ping_ < now) {
      next_ping_ = now + shared_.settings.ws.ping_freq;
      ping(now);
    }
  }
}

void DropCopyUsdM::operator()(metrics::Writer &writer) const {
  writer
      // counter
      .write(counter_.disconnect, metrics::Type::COUNTER)
      // profile
      .write(profile_.parse, metrics::Type::PROFILE)
      // latency
      .write(latency_.ping, metrics::Type::LATENCY)
      .write(latency_.heartbeat, metrics::Type::LATENCY);
}

// web::socket::Client::Handler

void DropCopyUsdM::operator()(web::socket::Client::Connected const &) {
  assert(logon_timeout_.count() == 0);
  auto now = clock::get_system();
  logon_timeout_ = now + shared_.settings.ws.request_timeout;
}

void DropCopyUsdM::operator()(web::socket::Client::Disconnected const &) {
  ++counter_.disconnect;
  ready_ = false;
  (*this)(ConnectionStatus::DISCONNECTED);
  logon_timeout_ = {};
  next_ping_ = {};
}

void DropCopyUsdM::operator()(web::socket::Client::Ready const &) {
  login();
}

void DropCopyUsdM::operator()(web::socket::Client::Close const &) {
}

void DropCopyUsdM::operator()(web::socket::Client::Latency const &latency) {
  TraceInfo trace_info;
  auto external_latency = ExternalLatency{
      .stream_id = stream_id_,
      .account = account_.name,
      .latency = latency.sample,
  };
  create_trace_and_dispatch(handler_, trace_info, external_latency);
  latency_.ping.update(latency.sample);
}

void DropCopyUsdM::operator()(web::socket::Client::Text const &text) {
  parse(text.payload);
}

void DropCopyUsdM::operator()(web::socket::Client::Binary const &) {
  log::fatal("Unexpected"sv);
}

void DropCopyUsdM::operator()(ConnectionStatus connection_status, std::string_view const &reason) {
  connection_status_ = connection_status;
  TraceInfo trace_info;
  auto stream_status = StreamStatus{
      .stream_id = stream_id_,
      .account = account_.name,
      .supports = SUPPORTS,
      .transport = Transport::TCP,
      .protocol = Protocol::WS,
      .encoding = {Encoding::JSON},
      .priority = Priority::PRIMARY,
      .connection_status = connection_status_,
      .reason = reason,
      .interface = (*connection_).get_interface(),
      .authority = (*connection_).get_current_authority(),
      .path = (*connection_).get_current_path(),
      .proxy = (*connection_).get_proxy(),
  };
  log::info("stream_status={}"sv, stream_status);
  create_trace_and_dispatch(handler_, trace_info, stream_status);
}

void DropCopyUsdM::ping(std::chrono::nanoseconds now) {
  auto message = fmt::format(
      R"({{)"
      R"("id":{},)"
      R"("method":"server.ping",)"
      R"("params":[])"
      R"(}})"sv,
      std::chrono::duration_cast<std::chrono::milliseconds>(now).count());
  (*connection_).send_text(message);
}

void DropCopyUsdM::login() {
  auto message = account_.create_ws_login(REQUEST_ID_AUTH);
  // log::warn("DEBUG message={}"sv, message);
  (*connection_).send_text(message);
}

void DropCopyUsdM::subscribe() {
  subscribe(REQUEST_ID_AOP, shared_.api.order_management.accounts_orders_positions);
}

void DropCopyUsdM::subscribe(uint64_t id, std::string_view const &topic) {
  log::info(R"(Subscribe topic="{}")"sv, topic);
  auto message = fmt::format(
      R"({{)"
      R"("id":{},)"
      R"("method":"{}.subscribe",)"
      R"("params":[])"
      R"(}})"sv,
      id,
      topic);
  log::debug("message={}"sv, message);
  (*connection_).send_text(message);
}

void DropCopyUsdM::parse(std::string_view const &message) {
  profile_.parse([&]() {
    log::info<5>(R"(message="{}")"sv, message);
    auto log_message = [&]() { log::warn(R"(*** PLEASE REPORT *** message="{}")"sv, message); };
    try {
      TraceInfo trace_info;
      if (!json::Parser2::dispatch(*this, message, decode_buffer_, trace_info, shared_.settings.experimental.allow_unknown_event_types)) {
        log_message();
      }
    } catch (...) {
      log_message();
      utils::exceptions::Unhandled::terminate();
    }
  });
}

// json::Parser2::Handler

// - admin

void DropCopyUsdM::operator()(Trace<json::Pong> const &event) {
  auto &[trace_info, pong] = event;
  auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(trace_info.source_receive_time) - std::chrono::milliseconds{pong.id};
  log::debug("pong={} (latency={}))"sv, pong, latency);
}

// note! sometimes seeing this: {"error":{"code":6012,"message":"invalid login token"},"id":1,"result":null}
void DropCopyUsdM::operator()(Trace<json::Ack> const &event) {
  auto &[trace_info, ack] = event;
  auto success = ack.result.status == json::AckResultStatus::SUCCESS;
  auto auth_helper = [&]() {
    if (success) {
      subscribe();
      ready_ = true;
      (*this)(ConnectionStatus::READY);
    } else {
      log::error(R"(Login failed: code={}, message="{}")"sv, ack.error.code, ack.error.message);
      (*connection_).close();
    }
  };
  auto aop_helper = [&]() {
    if (!success) {
      log::warn(R"(Subscription failed: code={}, message="{}")"sv, ack.error.code, ack.error.message);
    }
  };
  switch (ack.id) {
    case REQUEST_ID_AUTH:
      auth_helper();
      break;
    case REQUEST_ID_AOP:
      aop_helper();
      break;
  }
}

// - market-data

void DropCopyUsdM::operator()(Trace<json::Orderbook> const &) {
  log::fatal("Unexpected"sv);
}

void DropCopyUsdM::operator()(Trace<json::Trades2> const &) {
  log::fatal("Unexpected"sv);
}

void DropCopyUsdM::operator()(Trace<json::Market24h2> const &) {
  log::fatal("Unexpected"sv);
}

void DropCopyUsdM::operator()(Trace<json::Kline2> const &) {
  log::fatal("Unexpected"sv);
}

// - drop-copy

void DropCopyUsdM::operator()(Trace<json::IndexMarket24h> const &event) {
  auto &[trace_info, index_market24h] = event;
  log::info<2>("index_market24h={}"sv, index_market24h);
}

void DropCopyUsdM::operator()(Trace<json::AccountsOrdersPositions2> const &event) {
  auto &[trace_info, accounts_orders_positions] = event;
  log::info<2>("accounts_orders_positions={}"sv, accounts_orders_positions);
  auto update_type = map(accounts_orders_positions.type).template get<UpdateType>();
  for (auto &item : accounts_orders_positions.accounts_p) {
    log::info<2>("item={}"sv, item);
    // XXX FIXME TODO hold = account_balance_rv - total_used_balance_rv ???
    auto external_account = fmt::format("{}"sv, item.account_id);
    auto funds_update = FundsUpdate{
        .stream_id = stream_id_,
        .account = account_.name,
        .currency = item.currency,
        .margin_mode = {},
        .balance = item.account_balance_rv,
        .hold = NaN,
        .borrowed = NaN,
        .external_account = external_account,
        .update_type = update_type,
        .exchange_time_utc = {},  // ???
        .exchange_sequence = utils::safe_cast(accounts_orders_positions.sequence),
        .sending_time_utc = accounts_orders_positions.timestamp,
    };
    create_trace_and_dispatch(handler_, trace_info, funds_update, true);
  }
  for (auto &item : accounts_orders_positions.orders_p) {
    log::info<2>("item={}"sv, item);
    /*
    log::warn("DEBUG item={}"sv, item);
    log::warn(
        "DEBUG exec_status={}, ord_status={}, leaves_qty={}, cum_qty={}, exec_qty={}, exec_price_rp={}, exec_fee_rv={}, exec_seq={}, last_liquidity_ind={},
    trade_type={}"sv, item.exec_status, item.ord_status, item.leaves_qty, item.cum_qty, item.exec_qty, item.exec_price_rp, item.exec_fee_rv, item.exec_seq,
        item.last_liquidity_ind,
        item.trade_type);
    */
    auto order_status = map(item.ord_status).template get<OrderStatus>();
    if (update_type == UpdateType::SNAPSHOT && utils::is_order_complete(order_status)) {  // download open orders
      continue;
    }
    auto external_account = fmt::format("{}"sv, item.account_id);
    auto exchange_or_request_id = [&]() {
      if (std::empty(item.cl_ord_id)) {
        return item.order_id;
      }
      return item.cl_ord_id;
    }();
    auto order_update = server::oms::OrderUpdate{
        .account = account_.name,
        .exchange = shared_.settings.exchange,
        .symbol = item.symbol,
        .side = map(item.side),
        .position_effect = {},
        .margin_mode = {},
        .max_show_quantity = NaN,
        .order_type = map(item.ord_type),
        .time_in_force = map(item.time_in_force),
        .execution_instructions = {},
        .create_time_utc = item.transact_time_ns,
        .update_time_utc = item.transact_time_ns,
        .external_account = external_account,
        .external_order_id = item.order_id,
        .client_order_id = item.cl_ord_id,
        .order_status = order_status,
        .error = {},
        .text = item.message,
        .quantity = item.order_qty,
        .price = item.price_rp,
        .stop_price = item.stop_px_rp,
        .leverage = NaN,
        .remaining_quantity = item.leaves_qty,
        .traded_quantity = item.cum_qty,  // cum_value_rv ???
        .average_traded_price = NaN,
        .last_traded_quantity = NaN,  // exec_qty, exec_value_rv ???
        .last_traded_price = NaN,     // exec_price_rp ???
        .last_liquidity = {},
        .routing_id = {},
        .max_request_version = {},
        .max_response_version = {},
        .max_accepted_version = {},
        .update_type = update_type,
        .sending_time_utc = accounts_orders_positions.timestamp,
    };
    auto user_id = SOURCE_NONE;
    auto order_id = ORDER_ID_NONE;
    auto strategy_id = STRATEGY_ID_NONE;
    if (shared_.update_order(exchange_or_request_id, stream_id_, trace_info, order_update, [&](auto &order) {
          user_id = order.user_id;
          order_id = order.order_id;
          strategy_id = order.strategy_id;
        })) {
    } else {
      log::warn("*** EXTERNAL ORDER *** ({} / {})"sv, item.order_id, exchange_or_request_id);
    }
    if (item.trade_type == json::TradeType::TRADE) {
      if (item.exec_status != json::ExecStatus::TAKER_FILL && item.exec_status != json::ExecStatus::MAKER_FILL) {
        log::fatal("Unexpected: {}"sv, item);
      }
      auto fill = Fill{
          .exchange_time_utc = item.transact_time_ns,
          .external_trade_id = {},
          .quantity = item.exec_qty,
          .price = item.exec_price_rp,
          .liquidity = map(item.last_liquidity_ind),
          .commission_amount = item.exec_fee_rv,
          .commission_currency = {},  // XXX FIXME TODO
          .base_amount = NaN,
          .quote_amount = NaN,
          .profit_loss_amount = NaN,
      };
      fmt::format_to(std::back_inserter(fill.external_trade_id), "{}"sv, item.exec_id);
      auto trade_update = TradeUpdate{
          .stream_id = stream_id_,
          .account = account_.name,
          .order_id = {},
          .exchange = shared_.settings.exchange,
          .symbol = item.symbol,
          .side = map(item.side),
          .position_effect = map(item.pos_side, item.side),
          .margin_mode = {},
          .create_time_utc = item.transact_time_ns,
          .update_time_utc = item.transact_time_ns,
          .external_account = external_account,
          .external_order_id = item.order_id,
          .client_order_id = item.cl_ord_id,
          .fills = {&fill, 1},
          .routing_id = {},
          .update_type = update_type,
          .exchange_time_utc = item.transact_time_ns,
          .exchange_sequence = utils::safe_cast(item.exec_seq),
          .sending_time_utc = accounts_orders_positions.timestamp,
          .user = {},
          .strategy_id = {},
      };
      create_trace_and_dispatch(handler_, trace_info, trade_update, true, user_id, exchange_or_request_id);
    }
  }
  for (auto &item : accounts_orders_positions.positions_p) {
    log::info<2>("item={}"sv, item);
    auto external_account = fmt::format("{}"sv, item.account_id);
    if (utils::compare(item.assigned_pos_balance_rv, 0.0) < 0) {
      log::error("*** PLEASE REPORT *** {}"sv, item);
      continue;
    }
    auto side = map(item.side).template get<Side>();
    auto long_quantity = side == Side::BUY ? item.assigned_pos_balance_rv : 0.0;
    auto short_quantity = side == Side::SELL ? item.assigned_pos_balance_rv : 0.0;
    auto position_update = PositionUpdate{
        .stream_id = stream_id_,
        .account = account_.name,
        .exchange = shared_.settings.exchange,
        .symbol = item.symbol,
        .margin_mode = {},  // ???
        .external_account = external_account,
        .long_quantity = long_quantity,
        .short_quantity = short_quantity,
        .update_type = update_type,
        .exchange_time_utc = item.transact_time_ns,
        .exchange_sequence = utils::safe_cast(item.exec_seq),
        .sending_time_utc = accounts_orders_positions.timestamp,
    };
    create_trace_and_dispatch(handler_, trace_info, position_update, true);
  }
}

void DropCopyUsdM::operator()(Trace<json::PositionInfo> const &event) {
  auto &[trace_info, position_info] = event;
  log::info<2>("position_info={}"sv, position_info);
}

}  // namespace phemex_futures
}  // namespace roq
