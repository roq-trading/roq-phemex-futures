/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/phemex_futures/json/encoder.hpp"

#include <fmt/format.h>

#include "roq/decimal.hpp"

#include "roq/phemex_futures/json/map.hpp"
#include "roq/phemex_futures/json/utils.hpp"

using namespace std::literals;

namespace roq {
namespace phemex_futures {
namespace json {

// TimeInForce POST_ONLY
// reduceOnly
// stpMode
std::string_view Encoder::create_order(
    std::string &buffer, CreateOrder const &create_order, server::oms::Order const &order, std::string_view const &request_id) {
  buffer.clear();
  fmt::format_to(
      std::back_inserter(buffer),
      "?clOrdID={}"
      "&symbol={}"
      "&reduceOnly=false"
      "&orderQtyRq={}"
      "&ordType=Limit"
      "&priceRp={}"
      "&side=Buy"
      "&posSide=Long"
      "&timeInForce=GoodTillCancel"sv,
      request_id,
      create_order.symbol,
      Decimal{create_order.quantity, order.quantity_precision.precision},
      Decimal{create_order.price, order.price_precision.precision});
  // "&stopPxRp={}"
  // "&stpInstruction=CancelBoth"
  return buffer;
}

std::string_view Encoder::modify_order(
    std::string &buffer, ModifyOrder const &modify_order, server::oms::Order const &order, [[maybe_unused]] std::string_view const &request_id) {
  buffer.clear();
  fmt::format_to(std::back_inserter(buffer), "?symbol={}"sv, order.symbol);
  if (std::empty(order.external_order_id)) {
    fmt::format_to(std::back_inserter(buffer), "&origClOrdID={}"sv, order.client_order_id);
  } else {
    fmt::format_to(std::back_inserter(buffer), "&orderID={}"sv, order.external_order_id);
  }
  if (!std::isnan(modify_order.price)) {
    fmt::format_to(std::back_inserter(buffer), "&priceRp={}"sv, Decimal{modify_order.price, order.price_precision.precision});
  }
  if (!std::isnan(modify_order.quantity)) {
    fmt::format_to(std::back_inserter(buffer), "&orderQtyRq={}"sv, Decimal{modify_order.quantity, order.quantity_precision.precision});
  }
  // stopPxRp  - new stop price, real value
  fmt::format_to(std::back_inserter(buffer), "&posSide=Long"sv);

  return buffer;
}

std::string_view Encoder::cancel_order(
    std::string &buffer, CancelOrder const &, server::oms::Order const &order, [[maybe_unused]] std::string_view const &request_id) {
  assert(!std::empty(symbol));
  buffer.clear();
  fmt::format_to(std::back_inserter(buffer), "?symbol={}"sv, order.symbol);
  if (std::empty(order.external_order_id)) {
    fmt::format_to(std::back_inserter(buffer), "&clOrdID={}"sv, order.client_order_id);
  } else {
    fmt::format_to(std::back_inserter(buffer), "&orderID={}"sv, order.external_order_id);
  }
  fmt::format_to(std::back_inserter(buffer), "&posSide=Long"sv);
  return buffer;
}

std::string_view Encoder::cancel_all_orders(
    std::string &buffer, CancelAllOrders const &, std::string_view const &symbol, [[maybe_unused]] std::string_view const &request_id) {
  buffer.clear();
  fmt::format_to(std::back_inserter(buffer), "?symbol={}"sv, symbol);
  return buffer;
}

}  // namespace json
}  // namespace phemex_futures
}  // namespace roq
