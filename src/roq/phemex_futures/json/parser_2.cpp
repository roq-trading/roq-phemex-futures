/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/phemex_futures/json/parser_2.hpp"

#include "roq/logging.hpp"

#include "roq/core/json/parser.hpp"

#include "roq/phemex_futures/json/message_field.hpp"

using namespace std::literals;

namespace roq {
namespace phemex_futures {
namespace json {

// === HELPERS ===

namespace {
auto const BIT_ID = uint16_t{1} << 0;
// market-data
auto const BIT_ORDERBOOK = uint16_t{1} << 1;
auto const BIT_TRADES = uint16_t{1} << 2;
auto const BIT_MARKET24H = uint16_t{1} << 3;
auto const BIT_KLINE = uint16_t{1} << 4;
// drop-copy
auto const BIT_INDEX_MARKET24H = uint16_t{1} << 5;
auto const BIT_ACCOUNTS_ORDERS_POSITIONS = uint16_t{1} << 6;
auto const BIT_POSITION_INFO = uint16_t{1} << 7;
}  // namespace

// === HELPERS ===

namespace {
template <typename T>
void dispatch_helper(auto &handler, auto &message, auto &buffer_stack, auto &trace_info) {
  T obj{message, buffer_stack};
  create_trace_and_dispatch(handler, trace_info, obj);
}
}  // namespace

// === IMPLEMENTATION ===

bool Parser2::dispatch(
    Handler &handler, std::string_view const &message, core::json::BufferStack &buffer_stack, TraceInfo const &trace_info, bool allow_unknown_event_types) {
  uint16_t mask = {};
  auto pong = true;
  auto helper = [&](auto &key, auto &value) {
    MessageField field{key};
    switch (field) {
      using enum MessageField::type_t;
      case UNDEFINED_INTERNAL:
        assert(false);
      case UNKNOWN_INTERNAL:
        break;
      case ID:
        mask |= BIT_ID;
        break;
      case ERROR:
      case RESULT:
        if (core::json::is_object(value)) {
          pong = false;
        }
        break;
      case SEQUENCE:
        break;
      case TIMESTAMP:
        break;
      case SYMBOL:
        break;
      case TYPE:
        break;
      case BOOK:
        break;
      case ORDERBOOK_P:
        mask |= BIT_ORDERBOOK;
        break;
      case TRADES:
        break;
      case TRADES_P:
        mask |= BIT_TRADES;
        break;
      case MARKET24H:
        break;
      case MARKET24H_P:
        mask |= BIT_MARKET24H;
        break;
      case KLINE:
        break;
      case KLINE_P:
        mask |= BIT_KLINE;
        break;
      case DEPTH:
        break;
      case PRICE_SCALE:
        break;
      case QTY_SCALE:
        break;
      case VALUE_SCALE:
        break;
      case VERSION:
        break;
      case INDEX_MARKET24H:
        mask |= BIT_INDEX_MARKET24H;
        break;
      case ACCOUNTS:
      case ORDERS:
      case POSITIONS:
        break;
      case ACCOUNTS_P:
      case ORDERS_P:
      case POSITIONS_P:
        mask |= BIT_ACCOUNTS_ORDERS_POSITIONS;
        break;
      case DTS:
        break;
      case MTS:
        break;
      case POSITION_INFO:
        mask |= BIT_POSITION_INFO;
        break;
    }
    return false;  // note! the protocol forces us to parse everything
  };
  core::json::Parser::dispatch<core::json::Object>(helper, message);
  if (mask == 0) [[unlikely]] {
    if (allow_unknown_event_types) {
      return false;
    }
  } else if (mask & BIT_ID) {
    assert((mask & (~BIT_ID)) == 0);
    if (pong) {
      dispatch_helper<Pong>(handler, message, buffer_stack, trace_info);
    } else {
      dispatch_helper<Ack>(handler, message, buffer_stack, trace_info);
    }
    return true;
  } else {
    assert(mask != 0);
    // market-data
    if (mask & BIT_ORDERBOOK) {
      dispatch_helper<Orderbook>(handler, message, buffer_stack, trace_info);
    }
    if (mask & BIT_TRADES) {
      dispatch_helper<Trades2>(handler, message, buffer_stack, trace_info);
    }
    if (mask & BIT_MARKET24H) {
      dispatch_helper<Market24h2>(handler, message, buffer_stack, trace_info);
    }
    if (mask & BIT_KLINE) {
      dispatch_helper<Kline2>(handler, message, buffer_stack, trace_info);
    }
    // drop-copy
    if (mask & BIT_INDEX_MARKET24H) {
      dispatch_helper<IndexMarket24h>(handler, message, buffer_stack, trace_info);
    }
    if (mask & BIT_ACCOUNTS_ORDERS_POSITIONS) {
      dispatch_helper<AccountsOrdersPositions2>(handler, message, buffer_stack, trace_info);
    }
    if (mask & BIT_POSITION_INFO) {
      dispatch_helper<PositionInfo>(handler, message, buffer_stack, trace_info);
    }
    return true;
  }
  log::fatal(R"(Unexpected: message="{}")"sv, message);
}

}  // namespace json
}  // namespace phemex_futures
}  // namespace roq
