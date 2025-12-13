/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <span>
#include <string_view>

#include "roq/trace_info.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/phemex_futures/json/ack.hpp"
#include "roq/phemex_futures/json/pong.hpp"

#include "roq/phemex_futures/json/kline2.hpp"
#include "roq/phemex_futures/json/market24h2.hpp"
#include "roq/phemex_futures/json/orderbook.hpp"
#include "roq/phemex_futures/json/trades2.hpp"

#include "roq/phemex_futures/json/accounts_orders_positions2.hpp"
#include "roq/phemex_futures/json/index_market24h.hpp"
#include "roq/phemex_futures/json/position_info.hpp"

namespace roq {
namespace phemex_futures {
namespace json {

struct Parser2 final {
  struct Handler {
    virtual void operator()(Trace<json::Pong> const &) = 0;
    virtual void operator()(Trace<json::Ack> const &) = 0;
    //
    virtual void operator()(Trace<json::Orderbook> const &) = 0;
    virtual void operator()(Trace<json::Trades2> const &) = 0;
    virtual void operator()(Trace<json::Market24h2> const &) = 0;
    virtual void operator()(Trace<json::Kline2> const &) = 0;
    //
    virtual void operator()(Trace<json::IndexMarket24h> const &) = 0;
    virtual void operator()(Trace<json::AccountsOrdersPositions2> const &) = 0;
    virtual void operator()(Trace<json::PositionInfo> const &) = 0;
  };

  static bool dispatch(Handler &, std::string_view const &message, core::json::BufferStack &, TraceInfo const &, bool allow_unknown_event_types);
};

}  // namespace json
}  // namespace phemex_futures
}  // namespace roq
