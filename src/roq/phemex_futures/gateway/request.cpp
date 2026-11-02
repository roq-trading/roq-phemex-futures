/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/phemex_futures/gateway/request.hpp"

#include "roq/logging.hpp"

using namespace std::literals;

namespace roq {
namespace phemex_futures {
namespace gateway {

// === IMPLEMENTATION ===

Request::Request(Shared &shared) : stale_orders{*this}, trades{*this}, shared_{shared} {
}

bool Request::ready() const {
  return stale_orders.ready() && trades.ready();
}

bool Request::can_request(TraceInfo const &trace_info) {
  return shared_.rate_limiter.can_request(trace_info);
}

}  // namespace gateway
}  // namespace phemex_futures
}  // namespace roq
