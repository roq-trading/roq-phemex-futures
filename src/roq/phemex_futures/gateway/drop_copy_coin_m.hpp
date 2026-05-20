/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string>
#include <string_view>

#include "roq/utils/metrics/counter.hpp"
#include "roq/utils/metrics/latency.hpp"
#include "roq/utils/metrics/profile.hpp"

#include "roq/io/context.hpp"

#include "roq/web/socket/client.hpp"

#include "roq/core/download.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/server.hpp"

#include "roq/phemex_futures/gateway/account.hpp"
#include "roq/phemex_futures/gateway/drop_copy.hpp"
#include "roq/phemex_futures/gateway/shared.hpp"

#include "roq/phemex_futures/json/parser.hpp"

namespace roq {
namespace phemex_futures {
namespace gateway {

struct DropCopyCoinM final : public DropCopy, public web::socket::Client::Handler, json::Parser::Handler {
  DropCopyCoinM(DropCopy::Handler &, io::Context &, uint16_t stream_id, Account &, Shared &);

  bool ready() const;

  void operator()(Event<Start> const &) override;
  void operator()(Event<Stop> const &) override;
  void operator()(Event<Timer> const &) override;

  void operator()(metrics::Writer &) const override;

 protected:
  // web::socket::Client::Handler
  void operator()(web::socket::Client::Connected const &) override;
  void operator()(web::socket::Client::Disconnected const &) override;
  void operator()(web::socket::Client::Ready const &) override;
  void operator()(web::socket::Client::Close const &) override;
  void operator()(web::socket::Client::Latency const &) override;
  void operator()(web::socket::Client::Text const &) override;
  void operator()(web::socket::Client::Binary const &) override;

  // json::Parser::Handler
  // - admin
  void operator()(Trace<json::Pong> const &) override;
  void operator()(Trace<json::Ack> const &) override;
  // - market-data
  void operator()(Trace<json::Book> const &) override;
  void operator()(Trace<json::Trades> const &) override;
  void operator()(Trace<json::Market24h> const &) override;
  void operator()(Trace<json::Kline> const &) override;
  // - drop-copy
  void operator()(Trace<json::IndexMarket24h> const &) override;
  void operator()(Trace<json::AccountsOrdersPositions> const &) override;
  void operator()(Trace<json::PositionInfo> const &) override;

 private:
  void operator()(ConnectionStatus, std::string_view const &reason = {});

  void ping(std::chrono::nanoseconds now);

  void login();

  void subscribe();

  void subscribe(uint64_t id, std::string_view const &topic);

  void parse(std::string_view const &message);

 private:
  DropCopy::Handler &handler_;
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
