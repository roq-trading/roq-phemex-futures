/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <span>
#include <string_view>

#include "roq/trace_info.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/phemex_futures/protocol/json/ack.hpp"
#include "roq/phemex_futures/protocol/json/pong.hpp"

#include "roq/phemex_futures/protocol/json/kline2.hpp"
#include "roq/phemex_futures/protocol/json/market24h2.hpp"
#include "roq/phemex_futures/protocol/json/orderbook.hpp"
#include "roq/phemex_futures/protocol/json/trades2.hpp"

#include "roq/phemex_futures/protocol/json/accounts_orders_positions2.hpp"
#include "roq/phemex_futures/protocol/json/index_market24h.hpp"
#include "roq/phemex_futures/protocol/json/position_info.hpp"

namespace roq {
namespace phemex_futures {
namespace protocol {
namespace json {

struct Parser2 final {
  struct Handler {
    virtual void operator()(Trace<protocol::json::Pong> const &) = 0;
    virtual void operator()(Trace<protocol::json::Ack> const &) = 0;
    //
    virtual void operator()(Trace<protocol::json::Orderbook> const &) = 0;
    virtual void operator()(Trace<protocol::json::Trades2> const &) = 0;
    virtual void operator()(Trace<protocol::json::Market24h2> const &) = 0;
    virtual void operator()(Trace<protocol::json::Kline2> const &) = 0;
    //
    virtual void operator()(Trace<protocol::json::IndexMarket24h> const &) = 0;
    virtual void operator()(Trace<protocol::json::AccountsOrdersPositions2> const &) = 0;
    virtual void operator()(Trace<protocol::json::PositionInfo> const &) = 0;
  };

  static bool dispatch(Handler &, std::string_view const &message, core::json::BufferStack &, TraceInfo const &, bool allow_unknown_event_types);
};

}  // namespace json
}  // namespace protocol
}  // namespace phemex_futures
}  // namespace roq
