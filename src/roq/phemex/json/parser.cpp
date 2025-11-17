/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/phemex/json/parser.hpp"

#include "roq/logging.hpp"

#include "roq/core/json/parser.hpp"

#include "roq/phemex/json/message_field.hpp"

using namespace std::literals;

namespace roq {
namespace phemex {
namespace json {

// === HELPERS ===

namespace {
auto const BIT_ID = uint8_t{1} << 0;
// market-data
auto const BIT_BOOK = uint8_t{1} << 1;
auto const BIT_TRADES = uint8_t{1} << 2;
auto const BIT_MARKET24H = uint8_t{1} << 3;
auto const BIT_KLINE = uint8_t{1} << 4;
// drop-copy
auto const BIT_INDEX_MARKET24H = uint8_t{1} << 5;
auto const BIT_ACCOUNTS_ORDERS_POSITIONS = uint8_t{1} << 6;
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

bool Parser::dispatch(
    Handler &handler, std::string_view const &message, core::json::BufferStack &buffer_stack, TraceInfo const &trace_info, bool allow_unknown_event_types) {
  uint8_t mask = {};
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
        mask |= BIT_BOOK;
        break;
      case TRADES:
        mask |= BIT_TRADES;
        break;
      case MARKET24H:
        mask |= BIT_MARKET24H;
        break;
      case INDEX_MARKET24H:
        mask |= BIT_INDEX_MARKET24H;
        break;
      case KLINE:
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
      case ACCOUNTS:
      case ORDERS:
      case POSITIONS:
        mask |= BIT_ACCOUNTS_ORDERS_POSITIONS;
        break;
    }
  };
  core::json::Parser parser{message};
  auto value = parser.root();
  std::get<core::json::Object>(value).dispatch(helper);
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
    if (mask & BIT_BOOK) {
      dispatch_helper<Book>(handler, message, buffer_stack, trace_info);
    }
    if (mask & BIT_TRADES) {
      dispatch_helper<Trades>(handler, message, buffer_stack, trace_info);
    }
    if (mask & BIT_MARKET24H) {
      dispatch_helper<Market24h>(handler, message, buffer_stack, trace_info);
    }
    if (mask & BIT_KLINE) {
      dispatch_helper<Kline>(handler, message, buffer_stack, trace_info);
    }
    // drop-copy
    if (mask & BIT_INDEX_MARKET24H) {
      dispatch_helper<IndexMarket24h>(handler, message, buffer_stack, trace_info);
    }
    if (mask & BIT_ACCOUNTS_ORDERS_POSITIONS) {
      dispatch_helper<AccountsOrdersPositions>(handler, message, buffer_stack, trace_info);
    }
    return true;
  }
  log::fatal(R"(Unexpected: message="{}")"sv, message);
}

}  // namespace json
}  // namespace phemex
}  // namespace roq
