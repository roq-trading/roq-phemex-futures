/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include <span>
#include <string_view>

#include "roq/trace_info.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/phemex/json/ack.hpp"
#include "roq/phemex/json/pong.hpp"

#include "roq/phemex/json/book.hpp"
#include "roq/phemex/json/kline.hpp"
#include "roq/phemex/json/market24h.hpp"
#include "roq/phemex/json/trades.hpp"

#include "roq/phemex/json/accounts_orders_positions.hpp"
#include "roq/phemex/json/index_market24h.hpp"

namespace roq {
namespace phemex {
namespace json {

struct Parser final {
  struct Handler {
    virtual void operator()(Trace<json::Pong> const &) = 0;
    virtual void operator()(Trace<json::Ack> const &) = 0;
    //
    virtual void operator()(Trace<json::Book> const &) = 0;
    virtual void operator()(Trace<json::Trades> const &) = 0;
    virtual void operator()(Trace<json::Market24h> const &) = 0;
    virtual void operator()(Trace<json::Kline> const &) = 0;
    //
    virtual void operator()(Trace<json::IndexMarket24h> const &) = 0;
    virtual void operator()(Trace<json::AccountsOrdersPositions> const &) = 0;
  };

  static bool dispatch(Handler &, std::string_view const &message, core::json::BufferStack &, TraceInfo const &, bool allow_unknown_event_types);
};

}  // namespace json
}  // namespace phemex
}  // namespace roq
