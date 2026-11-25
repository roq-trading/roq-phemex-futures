/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string>

#include "roq/utils/metrics/counter.hpp"
#include "roq/utils/metrics/latency.hpp"
#include "roq/utils/metrics/profile.hpp"

#include "roq/io/context.hpp"

#include "roq/web/rest/client.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/server.hpp"

#include "roq/server/stream.hpp"

#include "roq/phemex_futures/gateway/account.hpp"
#include "roq/phemex_futures/gateway/request.hpp"
#include "roq/phemex_futures/gateway/shared.hpp"

#include "roq/phemex_futures/protocol/json/get_order_ack2.hpp"
#include "roq/phemex_futures/protocol/json/orders_all_ack2.hpp"
#include "roq/phemex_futures/protocol/json/orders_cancel_ack2.hpp"
#include "roq/phemex_futures/protocol/json/orders_create_ack2.hpp"
#include "roq/phemex_futures/protocol/json/orders_replace_ack2.hpp"

namespace roq {
namespace phemex_futures {
namespace gateway {

struct OrderEntryUsdM final : public Base<OrderEntryUsdM>, public server::OrderActionStream, public web::rest::Client::Handler {
  struct Handler {};

  OrderEntryUsdM(Handler &, io::Context &, uint16_t stream_id, Account &, Shared &, Request &, bool master);

 protected:
  friend base_type;

  // server::Stream

  void operator()(metrics::Writer &) const override;

  void operator()(Trace<Start> const &) override;
  void operator()(Trace<Stop> const &) override;
  void operator()(Trace<Timer> const &) override;

  void operator()(Trace<ConnectionStatus> const &, std::string_view const &reason) override;

  // server::OrderActionStream

  void operator()(Trace<server::oms::OrderActionBatchBegin> const &) override;
  void operator()(Trace<server::oms::OrderActionBatchCommit> const &) override;
  void operator()(Trace<server::oms::OrderActionBatchRollback> const &) override;

  void operator()(Trace<CreateOrder> const &, server::oms::Order const &, server::oms::RefData const &, std::string_view const &request_id) override;
  void operator()(
      Trace<ModifyOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id) override;
  void operator()(
      Trace<CancelOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id) override;

  void operator()(Trace<CancelAllOrders> const &, std::string_view const &request_id) override;

  // web::rest::Client::Handler

  void operator()(Trace<web::rest::Connected> const &) override;
  void operator()(Trace<web::rest::Disconnected> const &) override;
  void operator()(Trace<web::rest::Latency> const &) override;
  bool get_ping_request(web::rest::Request &) override;

  // orders-create

  void orders_create(Trace<CreateOrder> const &, server::oms::Order const &, server::oms::RefData const &, std::string_view const &request_id);
  void orders_create_ack(Trace<web::rest::Response> const &, uint8_t user_id, uint64_t order_id, uint32_t version);
  void operator()(Trace<protocol::json::OrdersCreateAck2> const &, uint8_t user_id, uint64_t order_id, uint32_t version);

  // orders-replace

  void orders_replace(
      Trace<ModifyOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id);
  void orders_replace_ack(Trace<web::rest::Response> const &, uint8_t user_id, uint64_t order_id, uint32_t version);
  void operator()(Trace<protocol::json::OrdersReplaceAck2> const &, uint8_t user_id, uint64_t order_id, uint32_t version);

  // orders-cancel

  void orders_cancel(
      Trace<CancelOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id);
  void orders_cancel_ack(Trace<web::rest::Response> const &, uint8_t user_id, uint64_t order_id, uint32_t version);
  void operator()(Trace<protocol::json::OrdersCancelAck2> const &, uint8_t user_id, uint64_t order_id, uint32_t version);

  // orders-all

  void orders_all(Trace<CancelAllOrders> const &, std::string_view const &request_id);
  void orders_all_ack(Trace<web::rest::Response> const &);
  void operator()(Trace<protocol::json::OrdersAllAck2> const &);

  // order

  void get_order(TraceInfo const &, std::string_view const &client_order_id, std::string_view const &external_order_id, std::string_view const &symbol);
  void get_order_ack(Trace<web::rest::Response> const &, std::string_view const &client_order_id);
  void operator()(Trace<protocol::json::GetOrderAck2> const &);

  // user-fills

  void get_user_fills(TraceInfo const &);
  void get_user_fills_ack(Trace<web::rest::Response> const &);
  // void operator()(Trace<protocol::json::GetUserFillsAck2> const &);

  // helpers

  void check_request_queue(TraceInfo const &);

  void process_response(Trace<web::rest::Response> const &, auto error_handler, auto success_handler);

 private:
  [[maybe_unused]] Handler &handler_;
  //
  std::string const name_;
  bool const master_;
  std::unique_ptr<web::rest::Client> const connection_;
  //
  core::json::BufferStack decode_buffer_;
  struct {
    utils::metrics::Counter disconnect;
  } counter_;
  struct {
    utils::metrics::Profile  //
        orders_create,
        orders_create_ack,                   //
        orders_replace, orders_replace_ack,  //
        orders_cancel, orders_cancel_ack,    //
        orders_all, orders_all_ack,          //
        order, order_ack,                    //
        user_fills, user_fills_ack;
  } profile_;
  struct {
    utils::metrics::Latency ping;
  } latency_;
  Account &account_;
  Shared &shared_;
  Request &request_;
  std::string encode_buffer_;
  size_t download_stale_orders_counter_ = {};
};

}  // namespace gateway
}  // namespace phemex_futures
}  // namespace roq
