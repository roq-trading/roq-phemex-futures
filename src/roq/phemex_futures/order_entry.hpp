/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string_view>

#include "roq/server.hpp"

namespace roq {
namespace phemex_futures {

struct OrderEntry {
  struct Handler {
    virtual void operator()(Trace<StreamStatus> const &) = 0;
    virtual void operator()(Trace<ExternalLatency> const &) = 0;
    virtual void operator()(Trace<TradeUpdate> const &, bool is_last, uint8_t user_id, std::string_view const &request_id) = 0;
  };

  OrderEntry() = default;

  OrderEntry(OrderEntry const &) = delete;

  virtual ~OrderEntry() = default;

  virtual bool ready() const = 0;

  virtual void operator()(Event<Start> const &) = 0;
  virtual void operator()(Event<Stop> const &) = 0;
  virtual void operator()(Event<Timer> const &) = 0;

  virtual void operator()(metrics::Writer &) const = 0;

  virtual uint16_t operator()(Event<CreateOrder> const &, server::oms::Order const &, server::oms::RefData const &, std::string_view const &request_id) = 0;
  virtual uint16_t operator()(
      Event<ModifyOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id) = 0;
  virtual uint16_t operator()(
      Event<CancelOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id) = 0;

  virtual uint16_t operator()(Event<CancelAllOrders> const &, std::string_view const &request_id) = 0;
};

}  // namespace phemex_futures
}  // namespace roq
