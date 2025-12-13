/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/phemex_futures/market_data_coin_m.hpp"

#include <algorithm>

#include "roq/logging.hpp"

#include "roq/mask.hpp"

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
auto const NAME = "md"sv;

auto const SUPPORTS = Mask{
    SupportType::MARKET_BY_PRICE,
    SupportType::TRADE_SUMMARY,
    SupportType::STATISTICS,
};

uint64_t const REQUEST_ID = 1'000'000;

size_t const MAX_DECODE_BUFFER_DEPTH = 2;

auto const DEFAULT_KLINE_PERIOD = 60s;
}  // namespace

// === HELPERS ===

namespace {
auto create_name(auto stream_id) {
  return fmt::format("{}:{}"sv, stream_id, NAME);
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
      .query = {},
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

MarketDataCoinM::MarketDataCoinM(MarketData::Handler &handler, io::Context &context, uint16_t stream_id, Shared &shared, size_t index)
    : handler_{handler}, stream_id_{stream_id}, name_{create_name(stream_id_)}, index_{index}, connection_{create_connection(*this, shared.settings, context)},
      decode_buffer_{shared.settings.misc.decode_buffer_size, MAX_DECODE_BUFFER_DEPTH},
      counter_{
          .disconnect = create_metrics(shared.settings, name_, "disconnect"sv),
      },
      profile_{
          .parse = create_metrics(shared.settings, name_, "parse"sv),
          .pong = create_metrics(shared.settings, name_, "pong"sv),
          .ack = create_metrics(shared.settings, name_, "ack"sv),
          .book = create_metrics(shared.settings, name_, "book"sv),
          .trades = create_metrics(shared.settings, name_, "trades"sv),
          .market24h = create_metrics(shared.settings, name_, "market24h"sv),
          .kline = create_metrics(shared.settings, name_, "kline"sv),
      },
      latency_{
          .ping = create_metrics(shared.settings, name_, "ping"sv),
      },
      shared_{shared}, request_id_{static_cast<uint64_t>(stream_id_) * REQUEST_ID} {
}

void MarketDataCoinM::operator()(Event<Start> const &) {
  (*connection_).start();
}

void MarketDataCoinM::operator()(Event<Stop> const &) {
  (*connection_).stop();
}

void MarketDataCoinM::operator()(Event<Timer> const &event) {
  auto now = event.value.now;
  (*connection_).refresh(now);
  if (ready()) {
    if (next_ping_ < now) {
      next_ping_ = now + shared_.settings.ws.ping_freq;
      ping(now);
    }
    check_subscribe_queue(now);
  }
}

void MarketDataCoinM::operator()(metrics::Writer &writer) const {
  writer
      // counter
      .write(counter_.disconnect, metrics::Type::COUNTER)
      // profile
      .write(profile_.parse, metrics::Type::PROFILE)
      .write(profile_.pong, metrics::Type::PROFILE)
      .write(profile_.ack, metrics::Type::PROFILE)
      .write(profile_.book, metrics::Type::PROFILE)
      .write(profile_.trades, metrics::Type::PROFILE)
      .write(profile_.market24h, metrics::Type::PROFILE)
      .write(profile_.kline, metrics::Type::PROFILE)
      // latency
      .write(latency_.ping, metrics::Type::LATENCY);
}

void MarketDataCoinM::subscribe(size_t start_from) {
  if (ready()) {
    subscribe(shared_.symbols.get_slice(index_, start_from));
  }
}

// web::socket::Client::Handler

void MarketDataCoinM::operator()(web::socket::Client::Connected const &) {
}

void MarketDataCoinM::operator()(web::socket::Client::Disconnected const &) {
  ++counter_.disconnect;
  (*this)(ConnectionStatus::DISCONNECTED);
  subscribe_queue_.clear();
}

void MarketDataCoinM::operator()(web::socket::Client::Ready const &) {
  (*this)(ConnectionStatus::READY);
  subscribe();
}

void MarketDataCoinM::operator()(web::socket::Client::Close const &) {
}

void MarketDataCoinM::operator()(web::socket::Client::Latency const &latency) {
  TraceInfo trace_info;
  auto external_latency = ExternalLatency{
      .stream_id = stream_id_,
      .account = {},
      .latency = latency.sample,
  };
  create_trace_and_dispatch(handler_, trace_info, external_latency);
  latency_.ping.update(latency.sample);
}

void MarketDataCoinM::operator()(web::socket::Client::Text const &text) {
  parse(text.payload);
}

void MarketDataCoinM::operator()(web::socket::Client::Binary const &) {
  log::fatal("Unexpected"sv);
}

void MarketDataCoinM::operator()(ConnectionStatus status) {
  if (utils::update(status_, status)) {
    TraceInfo trace_info;
    auto stream_status = StreamStatus{
        .stream_id = stream_id_,
        .account = {},
        .supports = SUPPORTS,
        .transport = Transport::TCP,
        .protocol = Protocol::WS,
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

void MarketDataCoinM::ping(std::chrono::nanoseconds now) {
  auto message = fmt::format(
      R"({{)"
      R"("id":{},)"
      R"("method":"server.ping",)"
      R"("params":[])"
      R"(}})"sv,
      std::chrono::duration_cast<std::chrono::milliseconds>(now).count());
  (*connection_).send_text(message);
}

void MarketDataCoinM::subscribe(std::span<Symbol const> const &symbols) {
  for (auto &item : symbols) {
    subscribe(item, shared_.api.market_data.orderbook, shared_.settings.ws.mbp_depth);
    subscribe(item, shared_.api.market_data.trade);
    subscribe(item, shared_.api.market_data.market24h);
    // subscribe(item, shared_.api.market_data.kline, DEFAULT_KLINE_PERIOD);
  }
}

void MarketDataCoinM::subscribe(Symbol const &symbol, std::string_view const &topic) {
  auto message = fmt::format(
      R"({{)"
      R"("id":{},)"
      R"("method":"{}.subscribe",)"
      R"("params":[)"
      R"("{}")"
      R"(])"
      R"(}})"sv,
      ++request_id_,
      topic,
      symbol);
  // log::warn(R"(DEBUG message="{}")"sv, message);
  subscribe_queue_.emplace_back(message);
}

// note! book
void MarketDataCoinM::subscribe(Symbol const &symbol, std::string_view const &topic, uint32_t depth) {
  switch (depth) {
    case 0:  // full
    case 1:
    case 5:
    case 10:
    case 30:
      break;
    default:
      log::fatal("Unsupported: depth={}"sv, depth);
  }
  auto message = fmt::format(
      R"({{)"
      R"("id":{},)"
      R"("method":"{}.subscribe",)"
      R"("params":[)"
      R"("{}",)"
      R"(false,)"  // note! false=20ms, true=120ms
      R"({})"
      R"(])"
      R"(}})"sv,
      ++request_id_,
      topic,
      symbol,
      depth);
  // log::warn(R"(DEBUG message="{}")"sv, message);
  subscribe_queue_.emplace_back(message);
}

// note! kline
void MarketDataCoinM::subscribe(Symbol const &symbol, std::string_view const &topic, std::chrono::seconds interval) {
  auto message = fmt::format(
      R"({{)"
      R"("id":{},)"
      R"("method":"{}.subscribe",)"
      R"("params":[)"
      R"("{}",)"
      R"({})"
      R"(])"
      R"(}})"sv,
      ++request_id_,
      topic,
      symbol,
      interval.count());
  // log::warn(R"(DEBUG message="{}")"sv, message);
  subscribe_queue_.emplace_back(message);
}

void MarketDataCoinM::parse(std::string_view const &message) {
  profile_.parse([&]() {
    log::info<5>(R"(message="{}")"sv, message);
    auto log_message = [&]() { log::warn(R"(*** PLEASE REPORT *** message="{}")"sv, message); };
    try {
      TraceInfo trace_info;
      if (!json::Parser::dispatch(*this, message, decode_buffer_, trace_info, shared_.settings.experimental.allow_unknown_event_types)) {
        log_message();
      }
    } catch (...) {
      log_message();
      utils::exceptions::Unhandled::terminate();
    }
  });
}

// json::Parser::Handler

// - admin

void MarketDataCoinM::operator()(Trace<json::Pong> const &event) {
  profile_.pong([&]() {
    auto &[trace_info, pong] = event;
    auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(trace_info.source_receive_time) - std::chrono::milliseconds{pong.id};
    // XXX HANS ExternalLatency
  });
}

void MarketDataCoinM::operator()(Trace<json::Ack> const &event) {
  profile_.ack([&]() {
    auto &[trace_info, ack] = event;
    if (ack.result.status != json::AckResultStatus::SUCCESS) {
      log::error("ack={}"sv, ack);
    }
  });
}

// - market-data

void MarketDataCoinM::operator()(Trace<json::Book> const &event) {
  profile_.book([&]() {
    auto &[trace_info, book] = event;
    log::info<3>("book={}"sv, book);
    auto helper = [&](auto &security) {
      auto &bids = shared_.bids;
      auto &asks = shared_.asks;
      bids.clear();
      asks.clear();
      for (auto &item : book.book.bids) {
        auto price = static_cast<double>(item.price_ep) / security.price_factor;
        auto quantity = static_cast<double>(item.qty) / security.quantity_factor;
        auto mbp_update = MBPUpdate{
            .price = price,
            .quantity = quantity,
            .implied_quantity = NaN,
            .number_of_orders = {},
            .update_action = {},
            .price_level = {},
        };
        bids.emplace_back(std::move(mbp_update));
      }
      for (auto &item : book.book.asks) {
        auto price = static_cast<double>(item.price_ep) / security.price_factor;
        auto quantity = static_cast<double>(item.qty) / security.quantity_factor;
        auto mbp_update = MBPUpdate{
            .price = price,
            .quantity = quantity,
            .implied_quantity = NaN,
            .number_of_orders = {},
            .update_action = {},
            .price_level = {},
        };
        asks.emplace_back(std::move(mbp_update));
      }
      if (!(std::empty(bids) && std::empty(asks))) {
        auto market_by_price_update = MarketByPriceUpdate{
            .stream_id = stream_id_,
            .exchange = shared_.settings.exchange,
            .symbol = book.symbol,
            .bids = bids,
            .asks = asks,
            .update_type = map(book.type),
            .exchange_time_utc = book.timestamp,
            .exchange_sequence = utils::safe_cast{book.sequence},
            .sending_time_utc = {},
            .price_precision = {},
            .quantity_precision = {},
            .max_depth = {},
            .checksum = {},
        };
        create_trace_and_dispatch(handler_, trace_info, market_by_price_update, true);
      }
    };
    if (shared_.find_security(book.symbol, helper)) {
    } else {
      log::warn("*** MISSING SYMBOL *** ({})"sv, book.symbol);
    }
  });
}

void MarketDataCoinM::operator()(Trace<json::Trades> const &event) {
  profile_.trades([&]() {
    auto &[trace_info, trades] = event;
    log::info<3>("trades={}"sv, trades);
    if (trades.type != json::MessageType::INCREMENTAL) {  // note! drop snapshot
      return;
    }
    if (!std::empty(trades.trades_p)) [[unlikely]] {
      log::fatal("Unexpected"sv);
    }
    auto &trades_2 = shared_.trades;
    trades_2.clear();
    auto helper = [&](auto &security) {
      using timestamp_type = decltype(json::TradesTradesItem::timestamp);
      auto timestamp = timestamp_type{};
      for (auto &item : trades.trades) {
        auto price = item.price_ep / security.price_factor;
        auto quantity = static_cast<double>(item.qty) / security.quantity_factor;
        auto item_2 = Trade{
            .side = map(item.side),
            .price = price,
            .quantity = quantity,
            .trade_id = {},  // note! nothing...
            .taker_order_id = {},
            .maker_order_id = {},
        };
        trades_2.emplace_back(std::move(item_2));
        utils::update_max(timestamp, item.timestamp);
      }
      if (!std::empty(trades_2)) {
        auto trade_summary = TradeSummary{
            .stream_id = stream_id_,
            .exchange = shared_.settings.exchange,
            .symbol = trades.symbol,
            .trades = trades_2,
            .exchange_time_utc = timestamp,
            .exchange_sequence = utils::safe_cast{trades.sequence},
            .sending_time_utc = {},
        };
        create_trace_and_dispatch(handler_, trace_info, trade_summary, true);
      }
    };
    if (shared_.find_security(trades.symbol, helper)) {
    } else {
      log::warn("*** MISSING SYMBOL *** ({})"sv, trades.symbol);
    }
  });
}

void MarketDataCoinM::operator()(Trace<json::Market24h> const &event) {
  profile_.market24h([&]() {
    auto &[trace_info, market24h] = event;
    log::info<3>("market24h={}"sv, market24h);
    auto helper = [&](auto &security) {
      std::array<Statistics, 8> statistics{{
          {
              .type = StatisticsType::OPEN_PRICE,
              .value = market24h.market24h.open / security.price_factor,
              .begin_time_utc = {},
              .end_time_utc = {},
          },
          {
              .type = StatisticsType::HIGHEST_TRADED_PRICE,
              .value = market24h.market24h.high / security.price_factor,
              .begin_time_utc = {},
              .end_time_utc = {},
          },
          {
              .type = StatisticsType::LOWEST_TRADED_PRICE,
              .value = market24h.market24h.low / security.price_factor,
              .begin_time_utc = {},
              .end_time_utc = {},
          },
          {
              .type = StatisticsType::INDEX_VALUE,
              .value = market24h.market24h.index_price / security.price_factor,
              .begin_time_utc = {},
              .end_time_utc = {},
          },
          {
              .type = StatisticsType::SETTLEMENT_PRICE,
              .value = market24h.market24h.mark_price / security.price_factor,
              .begin_time_utc = {},
              .end_time_utc = {},
          },
          {
              .type = StatisticsType::TRADE_VOLUME,
              .value = market24h.market24h.volume,  // XXX HANS convert ??? turnover ???
              .begin_time_utc = {},
              .end_time_utc = {},
          },
          {
              .type = StatisticsType::FUNDING_RATE,
              .value = market24h.market24h.pred_funding_rate,  // XXX HANS convert ???
              .begin_time_utc = {},
              .end_time_utc = {},
          },
          {
              .type = StatisticsType::OPEN_INTEREST,
              .value = market24h.market24h.open_interest,  // XXX HANS convert ???
              .begin_time_utc = {},
              .end_time_utc = {},
          },
      }};
      auto statistics_update = StatisticsUpdate{
          .stream_id = stream_id_,
          .exchange = shared_.settings.exchange,
          .symbol = market24h.market24h.symbol,
          .statistics = statistics,
          .update_type = UpdateType::INCREMENTAL,
          .exchange_time_utc = market24h.timestamp,  // ???
          .exchange_sequence = {},
          .sending_time_utc = {},
      };
      create_trace_and_dispatch(handler_, trace_info, statistics_update, true);
    };
    if (shared_.find_security(market24h.market24h.symbol, helper)) {
    } else {
      log::warn("*** MISSING SYMBOL *** ({})"sv, market24h.market24h.symbol);
    }
  });
}

void MarketDataCoinM::operator()(Trace<json::Kline> const &event) {
  profile_.kline([&]() {
    auto &[trace_info, kline] = event;
    log::info<3>("kline={}"sv, kline);
  });
}

// - drop-copy

void MarketDataCoinM::operator()(Trace<json::IndexMarket24h> const &) {
  log::fatal("Unexpected"sv);
}

void MarketDataCoinM::operator()(Trace<json::AccountsOrdersPositions> const &) {
  log::fatal("Unexpected"sv);
}

void MarketDataCoinM::operator()(Trace<json::PositionInfo> const &) {
  log::fatal("Unexpected"sv);
}

void MarketDataCoinM::check_subscribe_queue(std::chrono::nanoseconds now) {
  auto can_request = [&](auto now) { return shared_.rate_limiter.can_request(now); };
  auto request = [&](auto &message) { (*connection_).send_text(message); };
  subscribe_queue_.dispatch(can_request, request, now);
}

}  // namespace phemex_futures
}  // namespace roq
