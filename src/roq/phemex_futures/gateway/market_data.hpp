/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include "roq/api.hpp"

namespace roq {
namespace phemex_futures {
namespace gateway {

struct MarketData {
  struct Handler {};

  MarketData() = default;

  MarketData(MarketData const &) = delete;

  virtual ~MarketData() = default;

  virtual void operator()(Event<Start> const &) = 0;
  virtual void operator()(Event<Stop> const &) = 0;
  virtual void operator()(Event<Timer> const &) = 0;

  virtual void operator()(metrics::Writer &) const = 0;

  virtual void subscribe(size_t start_from = 0) = 0;
};

}  // namespace gateway
}  // namespace phemex_futures
}  // namespace roq
