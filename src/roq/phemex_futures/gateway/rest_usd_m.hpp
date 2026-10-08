/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string>

#include "roq/utils/metrics/counter.hpp"
#include "roq/utils/metrics/latency.hpp"
#include "roq/utils/metrics/profile.hpp"

#include "roq/io/context.hpp"

#include "roq/web/rest/client.hpp"

#include "roq/core/download_2.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/server.hpp"

#include "roq/server/stream.hpp"

#include "roq/phemex_futures/gateway/account.hpp"
#include "roq/phemex_futures/gateway/shared.hpp"

#include "roq/phemex_futures/protocol/json/products_ack.hpp"

namespace roq {
namespace phemex_futures {
namespace gateway {

struct RestUsdM final : public Base<RestUsdM>, public server::Stream, public web::rest::Client::Handler {
  struct SymbolsUpdate final {
    std::span<Symbol const> symbols;
  };

  struct Handler {
    virtual void operator()(SymbolsUpdate &) = 0;
  };

  RestUsdM(Handler &, io::Context &context, uint16_t stream_id, Shared &, Account &);

  // protected:
  friend base_type;

  // server::Stream

  uint16_t stream_id() const override { return stream_id_; }

  bool ready() const override { return connection_status_ == ConnectionStatus::READY; }

  void operator()(Event<Start> const &) override;
  void operator()(Event<Stop> const &) override;
  void operator()(Event<Timer> const &) override;

  void operator()(metrics::Writer &) const override;

  void operator()(Trace<ConnectionStatus> const &, std::string_view const &reason = {}) override;

  // web::rest::Client::Handler

  void operator()(Trace<web::rest::Connected> const &) override;
  void operator()(Trace<web::rest::Disconnected> const &) override;
  void operator()(Trace<web::rest::Latency> const &) override;
  bool get_ping_request(web::rest::Request &) override;

  // core::Download

  enum class State {
    UNDEFINED = 0,
    PRODUCTS,
    DONE,
  };

  int32_t download(Trace<State> const &);

  // products

  void get_products();
  void get_products_ack(Trace<web::rest::Response> const &, uint32_t sequence);
  void operator()(Trace<protocol::json::ProductsAck> const &);

  // helpers

  void process_response(Trace<web::rest::Response> const &, auto error_handler, auto success_handler);

 private:
  Handler &handler_;
  // config
  uint16_t const stream_id_;
  std::string const name_;
  // connection
  std::unique_ptr<web::rest::Client> connection_;
  // buffers
  core::json::BufferStack decode_buffer_;
  // metrics
  struct {
    utils::metrics::Counter disconnect;
  } counter_;
  struct {
    utils::metrics::Profile products, products_ack;
  } profile_;
  struct {
    utils::metrics::Latency ping;
  } latency_;
  // cache
  Shared &shared_;
  Account &account_;
  // state
  ConnectionStatus connection_status_ = {};
  core::Download2<State> download_;
};

}  // namespace gateway
}  // namespace phemex_futures
}  // namespace roq
