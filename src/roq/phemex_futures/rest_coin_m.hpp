/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "roq/utils/metrics/counter.hpp"
#include "roq/utils/metrics/latency.hpp"
#include "roq/utils/metrics/profile.hpp"

#include "roq/io/context.hpp"

#include "roq/web/rest/client.hpp"

#include "roq/core/download.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/server.hpp"

#include "roq/phemex_futures/rest.hpp"
#include "roq/phemex_futures/rest_state.hpp"
#include "roq/phemex_futures/shared.hpp"

#include "roq/phemex_futures/json/products_ack.hpp"

namespace roq {
namespace phemex_futures {

struct RestCoinM final : public Rest, public web::rest::Client::Handler {
  RestCoinM(Rest::Handler &, io::Context &context, uint16_t stream_id, Shared &);

  bool ready() const { return connection_status_ == ConnectionStatus::READY; }

  void operator()(Event<Start> const &) override;
  void operator()(Event<Stop> const &) override;
  void operator()(Event<Timer> const &) override;

  void operator()(metrics::Writer &) const override;

 protected:
  // web::rest::Client::Handler

  void operator()(Trace<web::rest::Client::Connected> const &) override;
  void operator()(Trace<web::rest::Client::Disconnected> const &) override;
  void operator()(Trace<web::rest::Client::Latency> const &) override;

  void operator()(ConnectionStatus, std::string_view const &reason = {});

  uint32_t download(RestState);

  // products

  void get_products();
  void get_products_ack(Trace<web::rest::Response> const &, uint32_t sequence);
  void operator()(Trace<json::ProductsAck> const &);

  void process_response(web::rest::Response const &, auto error_handler, auto success_handler);

 private:
  Rest::Handler &handler_;
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
  // state
  ConnectionStatus connection_status_ = {};
  core::Download<RestState> download_;
};

}  // namespace phemex_futures
}  // namespace roq
