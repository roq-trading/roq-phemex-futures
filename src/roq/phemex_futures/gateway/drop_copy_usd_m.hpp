/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string>

#include "roq/utils/metrics/counter.hpp"
#include "roq/utils/metrics/latency.hpp"
#include "roq/utils/metrics/profile.hpp"

#include "roq/io/context.hpp"

#include "roq/web/socket/client.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/server.hpp"

#include "roq/server/stream.hpp"

#include "roq/phemex_futures/gateway/account.hpp"
#include "roq/phemex_futures/gateway/shared.hpp"

#include "roq/phemex_futures/protocol/json/parser_2.hpp"

namespace roq {
namespace phemex_futures {
namespace gateway {

struct DropCopyUsdM final : Base<DropCopyUsdM>, public server::Stream, public web::socket::Client::Handler, protocol::json::Parser2::Handler {
  struct Handler {};

  DropCopyUsdM(Handler &, io::Context &, uint16_t stream_id, Account &, Shared &);

  // protected:
  friend base_type;

  // server::Stream

  uint16_t stream_id() const override { return stream_id_; }

  bool ready() const override;

  void operator()(Trace<Start> const &) override;
  void operator()(Trace<Stop> const &) override;
  void operator()(Trace<Timer> const &) override;

  void operator()(metrics::Writer &) const override;

  void operator()(Trace<ConnectionStatus> const &, std::string_view const &reason = {}) override;

  // web::socket::Client::Handler

  void operator()(Trace<web::socket::Connected> const &) override;
  void operator()(Trace<web::socket::Disconnected> const &) override;
  void operator()(Trace<web::socket::Latency> const &) override;
  void operator()(Trace<web::socket::Ready> const &) override;
  void operator()(Trace<web::socket::Close> const &) override;
  void operator()(Trace<web::socket::Text> const &) override;
  void operator()(Trace<web::socket::Binary> const &) override;

  // protocol::json::Parser2::Handler

  // - admin
  void operator()(Trace<protocol::json::Pong> const &) override;
  void operator()(Trace<protocol::json::Ack> const &) override;
  // - market-data
  void operator()(Trace<protocol::json::Orderbook> const &) override;
  void operator()(Trace<protocol::json::Trades2> const &) override;
  void operator()(Trace<protocol::json::Market24h2> const &) override;
  void operator()(Trace<protocol::json::Kline2> const &) override;
  // - drop-copy
  void operator()(Trace<protocol::json::IndexMarket24h> const &) override;
  void operator()(Trace<protocol::json::AccountsOrdersPositions2> const &) override;
  void operator()(Trace<protocol::json::PositionInfo> const &) override;

  // helpers

  void ping(std::chrono::nanoseconds now);

  void login();

  void subscribe();

  void subscribe(uint64_t id, std::string_view const &topic);

  void parse(std::string_view const &message);

 private:
  [[maybe_unused]] Handler &handler_;
  // config
  uint16_t const stream_id_;
  std::string const name_;
  // web socket
  std::unique_ptr<web::socket::Client> connection_;
  // buffers
  core::json::BufferStack decode_buffer_;
  // metrics
  struct {
    utils::metrics::Counter disconnect;
  } counter_;
  struct {
    utils::metrics::Profile parse;
  } profile_;
  struct {
    utils::metrics::Latency ping, heartbeat;
  } latency_;
  // account
  Account &account_;
  // cache
  Shared &shared_;
  // state
  bool ready_ = false;
  ConnectionStatus connection_status_ = {};
  std::chrono::nanoseconds logon_timeout_ = {};
  std::chrono::nanoseconds next_ping_ = {};
};

}  // namespace gateway
}  // namespace phemex_futures
}  // namespace roq
