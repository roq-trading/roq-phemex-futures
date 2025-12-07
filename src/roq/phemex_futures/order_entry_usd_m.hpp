/* Copyright (c) 2017-2025, Hans Erik Thrane */

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

#include "roq/phemex_futures/account.hpp"
#include "roq/phemex_futures/order_entry.hpp"
#include "roq/phemex_futures/order_entry_state.hpp"
#include "roq/phemex_futures/shared.hpp"

#include "roq/phemex_futures/json/orders_all_ack2.hpp"
#include "roq/phemex_futures/json/orders_cancel_ack2.hpp"
#include "roq/phemex_futures/json/orders_create_ack2.hpp"
#include "roq/phemex_futures/json/orders_replace_ack2.hpp"

namespace roq {
namespace phemex_futures {

struct OrderEntryUsdM final : public OrderEntry, public web::rest::Client::Handler {
  OrderEntryUsdM(OrderEntry::Handler &, io::Context &, uint16_t stream_id, Account &, Shared &, bool master);

  bool ready() const override;

  void operator()(Event<Start> const &) override;
  void operator()(Event<Stop> const &) override;
  void operator()(Event<Timer> const &) override;

  void operator()(metrics::Writer &) const override;

  uint16_t operator()(Event<CreateOrder> const &, server::oms::Order const &, std::string_view const &request_id) override;
  uint16_t operator()(
      Event<ModifyOrder> const &, server::oms::Order const &, std::string_view const &request_id, std::string_view const &previous_request_id) override;
  uint16_t operator()(
      Event<CancelOrder> const &, server::oms::Order const &, std::string_view const &request_id, std::string_view const &previous_request_id) override;

  uint16_t operator()(Event<CancelAllOrders> const &, std::string_view const &request_id) override;

 protected:
  // web::rest::Client::Handler

  void operator()(Trace<web::rest::Client::Connected> const &) override;
  void operator()(Trace<web::rest::Client::Disconnected> const &) override;
  void operator()(Trace<web::rest::Client::Latency> const &) override;

  void operator()(ConnectionStatus);

  uint32_t download(OrderEntryState);

  // orders-create

  void orders_create(Event<CreateOrder> const &, server::oms::Order const &, std::string_view const &request_id);
  void orders_create_ack(Trace<web::rest::Response> const &, uint8_t user_id, uint64_t order_id, uint32_t version);
  void operator()(Trace<json::OrdersCreateAck2> const &, uint8_t user_id, uint64_t order_id, uint32_t version);

  // orders-replace

  void orders_replace(Event<ModifyOrder> const &, server::oms::Order const &, std::string_view const &request_id, std::string_view const &previous_request_id);
  void orders_replace_ack(Trace<web::rest::Response> const &, uint8_t user_id, uint64_t order_id, uint32_t version);
  void operator()(Trace<json::OrdersReplaceAck2> const &, uint8_t user_id, uint64_t order_id, uint32_t version);

  // orders-cancel

  void orders_cancel(Event<CancelOrder> const &, server::oms::Order const &, std::string_view const &request_id, std::string_view const &previous_request_id);
  void orders_cancel_ack(Trace<web::rest::Response> const &, uint8_t user_id, uint64_t order_id, uint32_t version);
  void operator()(Trace<json::OrdersCancelAck2> const &, uint8_t user_id, uint64_t order_id, uint32_t version);

  // orders-all

  void orders_all(Event<CancelAllOrders> const &, std::string_view const &request_id);
  void orders_all_ack(Trace<web::rest::Response> const &, uint8_t user_id);
  void operator()(Trace<json::OrdersAllAck2> const &, uint8_t user_id);

  // helpers

  void process_response(web::rest::Response const &, auto error_handler, auto success_handler);

  template <typename... Args>
  void operator()(Trace<server::oms::Response> const &, uint8_t user_id, uint64_t order_id, Args &&...);

  void operator()(Trace<server::oms::OrderUpdate> const &, std::string_view const &client_order_id);

 private:
  OrderEntry::Handler &handler_;
  // config
  uint16_t const stream_id_;
  std::string const name_;
  bool const master_;
  // connection
  std::unique_ptr<web::rest::Client> connection_;
  // buffers
  core::json::BufferStack decode_buffer_;
  // metrics
  struct {
    utils::metrics::Counter disconnect;
  } counter_;
  struct {
    utils::metrics::Profile  //
        orders_create,
        orders_create_ack,                   //
        orders_replace, orders_replace_ack,  //
        orders_cancel, orders_cancel_ack,    //
        orders_all, orders_all_ack;
  } profile_;
  struct {
    utils::metrics::Latency ping;
  } latency_;
  // account
  Account &account_;
  Shared &shared_;
  // state
  ConnectionStatus status_ = {};
  core::Download<OrderEntryState> download_;
  //
  std::string encode_buffer_;
};

}  // namespace phemex_futures
}  // namespace roq
