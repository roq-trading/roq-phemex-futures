/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include <deque>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "roq/utils/metrics/counter.hpp"
#include "roq/utils/metrics/latency.hpp"
#include "roq/utils/metrics/profile.hpp"

#include "roq/io/context.hpp"

#include "roq/web/socket/client.hpp"

#include "roq/core/timer_queue.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/server.hpp"

#include "roq/phemex_futures/market_data.hpp"
#include "roq/phemex_futures/shared.hpp"

#include "roq/phemex_futures/json/parser.hpp"

namespace roq {
namespace phemex_futures {

struct MarketDataCoinM final : public MarketData, public web::socket::Client::Handler, public json::Parser::Handler {
  MarketDataCoinM(MarketData::Handler &, io::Context &, uint16_t stream_id, Shared &, size_t index);

  uint16_t stream_id() const { return stream_id_; }

  bool ready() const { return status_ == ConnectionStatus::READY; }

  void operator()(Event<Start> const &) override;
  void operator()(Event<Stop> const &) override;
  void operator()(Event<Timer> const &) override;

  void operator()(metrics::Writer &) const override;

  void subscribe(size_t start_from = 0) override;

 protected:
  // web::socket::client::Client::Handler
  void operator()(web::socket::Client::Connected const &) override;
  void operator()(web::socket::Client::Disconnected const &) override;
  void operator()(web::socket::Client::Ready const &) override;
  void operator()(web::socket::Client::Close const &) override;
  void operator()(web::socket::Client::Latency const &) override;
  void operator()(web::socket::Client::Text const &) override;
  void operator()(web::socket::Client::Binary const &) override;

 private:
  void operator()(ConnectionStatus);

  void ping(std::chrono::nanoseconds now);

  void subscribe(std::span<Symbol const> const &symbols);
  void subscribe(Symbol const &, std::string_view const &topic);
  void subscribe(Symbol const &, std::string_view const &topic, uint32_t depth);
  void subscribe(Symbol const &, std::string_view const &topic, std::chrono::seconds interval);

  void parse(std::string_view const &message);

  // json::Parser::Handler
  // - admin
  void operator()(Trace<json::Pong> const &) override;
  void operator()(Trace<json::Ack> const &) override;
  // - market-data
  void operator()(Trace<json::Book> const &) override;
  void operator()(Trace<json::Trades> const &) override;
  void operator()(Trace<json::Market24h> const &) override;
  void operator()(Trace<json::Market24h2> const &) override;
  void operator()(Trace<json::Kline> const &) override;
  // - drop-copy
  void operator()(Trace<json::IndexMarket24h> const &) override;
  void operator()(Trace<json::AccountsOrdersPositions> const &) override;
  void operator()(Trace<json::AccountsOrdersPositions2> const &) override;
  void operator()(Trace<json::PositionInfo> const &) override;

  void check_subscribe_queue(std::chrono::nanoseconds now);

 private:
  MarketData::Handler &handler_;
  // config
  uint16_t const stream_id_;
  std::string const name_;
  size_t const index_;
  // web socket
  std::unique_ptr<web::socket::Client> connection_;
  // buffers
  core::json::BufferStack decode_buffer_;
  // metrics
  struct {
    utils::metrics::Counter disconnect;
  } counter_;
  struct {
    utils::metrics::Profile parse,  //
        pong, ack,                  //
        book, trades, market24h, kline;
  } profile_;
  struct {
    utils::metrics::Latency ping;
  } latency_;
  // cache
  Shared &shared_;
  // state
  ConnectionStatus status_ = {};
  std::chrono::nanoseconds next_ping_ = {};
  uint64_t request_id_ = {};
  // queue
  core::TimerQueue<std::string> subscribe_queue_;
};

}  // namespace phemex_futures
}  // namespace roq
