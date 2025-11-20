/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/phemex_futures/market_data_usd_m.hpp"

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

MarketDataUsdM::MarketDataUsdM(MarketData::Handler &handler, io::Context &context, uint16_t stream_id, Shared &shared, size_t index)
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
      shared_{shared} {
}

void MarketDataUsdM::operator()(Event<Start> const &) {
  (*connection_).start();
}

void MarketDataUsdM::operator()(Event<Stop> const &) {
  (*connection_).stop();
}

void MarketDataUsdM::operator()(Event<Timer> const &event) {
  auto now = event.value.now;
  (*connection_).refresh(now);
  if (ready()) {
    if (next_ping_ < now) {
      next_ping_ = now + shared_.settings.ws.ping_freq;
      ping(now);
    }
  }
}

void MarketDataUsdM::operator()(metrics::Writer &writer) const {
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

void MarketDataUsdM::subscribe(size_t start_from) {
  if (ready()) {
    subscribe(shared_.symbols.get_slice(index_, start_from));
  }
}

// web::socket::Client::Handler

void MarketDataUsdM::operator()(web::socket::Client::Connected const &) {
}

void MarketDataUsdM::operator()(web::socket::Client::Disconnected const &) {
  ++counter_.disconnect;
  (*this)(ConnectionStatus::DISCONNECTED);
}

void MarketDataUsdM::operator()(web::socket::Client::Ready const &) {
  (*this)(ConnectionStatus::READY);
  subscribe();
}

void MarketDataUsdM::operator()(web::socket::Client::Close const &) {
}

void MarketDataUsdM::operator()(web::socket::Client::Latency const &latency) {
  TraceInfo trace_info;
  auto external_latency = ExternalLatency{
      .stream_id = stream_id_,
      .account = {},
      .latency = latency.sample,
  };
  create_trace_and_dispatch(handler_, trace_info, external_latency);
  latency_.ping.update(latency.sample);
}

void MarketDataUsdM::operator()(web::socket::Client::Text const &text) {
  parse(text.payload);
}

void MarketDataUsdM::operator()(web::socket::Client::Binary const &) {
  log::fatal("Unexpected"sv);
}

void MarketDataUsdM::operator()(ConnectionStatus status) {
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

void MarketDataUsdM::ping(std::chrono::nanoseconds now) {
  auto message = fmt::format(
      R"({{)"
      R"("id":{},)"
      R"("method":"server.ping",)"
      R"("params":[])"
      R"(}})"sv,
      std::chrono::duration_cast<std::chrono::milliseconds>(now).count());
  (*connection_).send_text(message);
}

void MarketDataUsdM::subscribe(std::span<Symbol const> const &symbols) {
  if (std::empty(symbols)) {
    return;
  }
  subscribe(symbols, shared_.api.market_data.orderbook, 0);
  subscribe(symbols, shared_.api.market_data.trade);
  subscribe(symbols, shared_.api.market_data.market24h);
  subscribe(symbols, shared_.api.market_data.kline, DEFAULT_KLINE_PERIOD);
}

void MarketDataUsdM::subscribe(std::span<Symbol const> const &symbols, std::string_view const &topic) {
  assert(!std::empty(symbols));
  for (auto &item : symbols) {
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
        item);
    // log::warn(R"(DEBUG message="{}")"sv, message);
    (*connection_).send_text(message);
  }
}

// note! book
void MarketDataUsdM::subscribe(std::span<Symbol const> const &symbols, std::string_view const &topic, uint32_t depth) {
  assert(!std::empty(symbols));
  for (auto &item : symbols) {
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
        item,
        depth);
    // log::warn(R"(DEBUG message="{}")"sv, message);
    (*connection_).send_text(message);
  }
}

// note! kline
void MarketDataUsdM::subscribe(std::span<Symbol const> const &symbols, std::string_view const &topic, std::chrono::seconds interval) {
  assert(!std::empty(symbols));
  for (auto &item : symbols) {
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
        item,
        interval.count());
    // log::warn(R"(DEBUG message="{}")"sv, message);
    (*connection_).send_text(message);
  }
}

void MarketDataUsdM::parse(std::string_view const &message) {
  profile_.parse([&]() {
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

void MarketDataUsdM::operator()(Trace<json::Pong> const &event) {
  profile_.pong([&]() {
    auto &[trace_info, pong] = event;
    auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(trace_info.source_receive_time) - std::chrono::milliseconds{pong.id};
    // XXX HANS ExternalLatency
  });
}

void MarketDataUsdM::operator()(Trace<json::Ack> const &event) {
  profile_.ack([&]() {
    auto &[trace_info, ack] = event;
    if (ack.result.status != json::AckResultStatus::SUCCESS) {
      log::warn("DEBUG ack={}"sv, ack);
    }
  });
}

// - market-data

void MarketDataUsdM::operator()(Trace<json::Book> const &event) {
  profile_.book([&]() {
    auto &[trace_info, book] = event;
    log::info<3>("book={}"sv, book);
    if (!std::empty(book.book.bids) || !std::empty(book.book.asks)) [[unlikely]] {
      log::fatal("Unexpected"sv);
    }
    auto &bids = shared_.bids;
    auto &asks = shared_.asks;
    bids.clear();
    asks.clear();
    for (auto &item : book.orderbook_p.bids) {
      auto mbp_update = MBPUpdate{
          .price = item.price_ep,  // XXX HANS convert to double
          .quantity = item.qty,
          .implied_quantity = NaN,
          .number_of_orders = {},
          .update_action = {},
          .price_level = {},
      };
      bids.emplace_back(std::move(mbp_update));
    }
    for (auto &item : book.orderbook_p.asks) {
      auto mbp_update = MBPUpdate{
          .price = item.price_ep,  // XXX HANS convert to double
          .quantity = item.qty,
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
  });
}

void MarketDataUsdM::operator()(Trace<json::Trades> const &event) {
  profile_.trades([&]() {
    auto &[trace_info, trades] = event;
    log::info<3>("trades={}"sv, trades);
    if (trades.type != json::MessageType::INCREMENTAL) {  // note! drop snapshot
      return;
    }
    if (!std::empty(trades.trades)) [[unlikely]] {
      log::fatal("Unexpected"sv);
    }
    auto &trades_2 = shared_.trades;
    trades_2.clear();
    using timestamp_type = decltype(json::TradesTradesItem::timestamp);
    auto timestamp = timestamp_type{};
    for (auto &item : trades.trades_p) {
      auto item_2 = Trade{
          .side = map(item.side),
          .price = item.price_ep,  // XXX HANS convert to double
          .quantity = item.qty,    // XXX HANS convert to double
          .trade_id = {},          // note! nothing...
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
  });
}

void MarketDataUsdM::operator()(Trace<json::Market24h> const &) {
  log::fatal("Unexpected"sv);
}

void MarketDataUsdM::operator()(Trace<json::Market24h2> const &event) {
  profile_.market24h([&]() {
    auto &[trace_info, market24h] = event;
    log::info<3>("market24h_p={}"sv, market24h);
    std::array<Statistics, 8> statistics{{
        {
            .type = StatisticsType::OPEN_PRICE,
            .value = market24h.market24h_p.open_rp,  // XXX HANS convert to double
            .begin_time_utc = {},
            .end_time_utc = {},
        },
        {
            .type = StatisticsType::HIGHEST_TRADED_PRICE,
            .value = market24h.market24h_p.high_rp,  // XXX HANS convert to double
            .begin_time_utc = {},
            .end_time_utc = {},
        },
        {
            .type = StatisticsType::LOWEST_TRADED_PRICE,
            .value = market24h.market24h_p.low_rp,  // XXX HANS convert to double
            .begin_time_utc = {},
            .end_time_utc = {},
        },
        {
            .type = StatisticsType::INDEX_VALUE,
            .value = market24h.market24h_p.index_price_rp,  // XXX HANS convert to double
            .begin_time_utc = {},
            .end_time_utc = {},
        },
        {
            .type = StatisticsType::SETTLEMENT_PRICE,
            .value = market24h.market24h_p.mark_price_rp,  // XXX HANS convert to double
            .begin_time_utc = {},
            .end_time_utc = {},
        },
        {
            .type = StatisticsType::TRADE_VOLUME,
            .value = market24h.market24h_p.volume_rq,  // XXX HANS convert ??? turnover ???
            .begin_time_utc = {},
            .end_time_utc = {},
        },
        {
            .type = StatisticsType::FUNDING_RATE,
            .value = market24h.market24h_p.pred_funding_rate_rr,  // XXX HANS convert ???
            .begin_time_utc = {},
            .end_time_utc = {},
        },
        {
            .type = StatisticsType::OPEN_INTEREST,
            .value = market24h.market24h_p.open_interest_rv,  // XXX HANS convert ???
            .begin_time_utc = {},
            .end_time_utc = {},
        },
    }};
    auto statistics_update = StatisticsUpdate{
        .stream_id = stream_id_,
        .exchange = shared_.settings.exchange,
        .symbol = market24h.market24h_p.symbol,
        .statistics = statistics,
        .update_type = UpdateType::INCREMENTAL,
        .exchange_time_utc = market24h.timestamp,  // ???
        .exchange_sequence = {},
        .sending_time_utc = {},
    };
    create_trace_and_dispatch(handler_, trace_info, statistics_update, true);
  });
}

void MarketDataUsdM::operator()(Trace<json::Kline> const &event) {
  profile_.kline([&]() {
    auto &[trace_info, kline] = event;
    log::info<3>("kline={}"sv, kline);
  });
}

// - drop-copy

void MarketDataUsdM::operator()(Trace<json::IndexMarket24h> const &) {
  log::fatal("Unexpected"sv);
}

void MarketDataUsdM::operator()(Trace<json::AccountsOrdersPositions> const &) {
  log::fatal("Unexpected"sv);
}

void MarketDataUsdM::operator()(Trace<json::AccountsOrdersPositions2> const &) {
  log::fatal("Unexpected"sv);
}

void MarketDataUsdM::operator()(Trace<json::PositionInfo> const &) {
  log::fatal("Unexpected"sv);
}

}  // namespace phemex_futures
}  // namespace roq
