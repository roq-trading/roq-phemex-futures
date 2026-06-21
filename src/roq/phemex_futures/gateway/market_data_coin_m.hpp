/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string>

#include "roq/utils/metrics/counter.hpp"
#include "roq/utils/metrics/latency.hpp"
#include "roq/utils/metrics/profile.hpp"

#include "roq/io/context.hpp"

#include "roq/web/socket/client.hpp"

#include "roq/core/timer_queue.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/server.hpp"

#include "roq/phemex_futures/gateway/market_data.hpp"
#include "roq/phemex_futures/gateway/shared.hpp"

#include "roq/phemex_futures/protocol/json/parser.hpp"

namespace roq {
namespace phemex_futures {
namespace gateway {

struct MarketDataCoinM final : public MarketData, public web::socket::Client::Handler, public protocol::json::Parser::Handler {
  MarketDataCoinM(MarketData::Handler &, io::Context &, uint16_t stream_id, Shared &, size_t index);

 protected:
  // MarketData

  void operator()(Event<Start> const &) override;
  void operator()(Event<Stop> const &) override;
  void operator()(Event<Timer> const &) override;

  void operator()(metrics::Writer &) const override;

  void subscribe(size_t start_from = 0) override;

  // web::socket::client::Client::Handler

  void operator()(web::socket::Client::Connected const &) override;
  void operator()(web::socket::Client::Disconnected const &) override;
  void operator()(web::socket::Client::Ready const &) override;
  void operator()(web::socket::Client::Close const &) override;
  void operator()(web::socket::Client::Latency const &) override;
  void operator()(web::socket::Client::Text const &) override;
  void operator()(web::socket::Client::Binary const &) override;

  // helpers

  uint16_t stream_id() const { return stream_id_; }

  bool ready() const { return connection_status_ == ConnectionStatus::READY; }

  void operator()(ConnectionStatus, std::string_view const &reason = {});

  void ping(std::chrono::nanoseconds now);

  void subscribe(std::span<Symbol const> const &symbols);
  void subscribe(Symbol const &, std::string_view const &topic);
  void subscribe(Symbol const &, std::string_view const &topic, uint32_t depth);
  void subscribe(Symbol const &, std::string_view const &topic, std::chrono::seconds interval);

  void parse(std::string_view const &message);

  // protocol::json::Parser::Handler
  // - admin
  void operator()(Trace<protocol::json::Pong> const &) override;
  void operator()(Trace<protocol::json::Ack> const &) override;
  // - market-data
  void operator()(Trace<protocol::json::Book> const &) override;
  void operator()(Trace<protocol::json::Trades> const &) override;
  void operator()(Trace<protocol::json::Market24h> const &) override;
  void operator()(Trace<protocol::json::Kline> const &) override;
  // - drop-copy
  void operator()(Trace<protocol::json::IndexMarket24h> const &) override;
  void operator()(Trace<protocol::json::AccountsOrdersPositions> const &) override;
  void operator()(Trace<protocol::json::PositionInfo> const &) override;

  // helpers

  void check_subscribe_queue(std::chrono::nanoseconds now);

 private:
  [[maybe_unused]] MarketData::Handler &handler_;
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
  ConnectionStatus connection_status_ = {};
  std::chrono::nanoseconds next_ping_ = {};
  uint64_t request_id_ = {};
  // queue
  core::TimerQueue<std::string> subscribe_queue_;
};

}  // namespace gateway
}  // namespace phemex_futures
}  // namespace roq
