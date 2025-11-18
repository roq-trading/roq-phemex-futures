/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/phemex_futures/json/encoder.hpp"

#include <fmt/format.h>

#include "roq/decimal.hpp"

#include "roq/logging.hpp"

#include "roq/phemex_futures/json/map.hpp"
#include "roq/phemex_futures/json/utils.hpp"

using namespace std::literals;

namespace roq {
namespace phemex_futures {
namespace json {

// no stp?
std::string_view Encoder::create_order_coin_m(
    std::string &buffer,
    CreateOrder const &create_order,
    server::oms::Order const &order,
    std::string_view const &request_id,
    tools::Security const &security) {
  buffer.clear();
  auto side = map(create_order.side).template get<json::Side>();
  auto ord_type = map(create_order.order_type).template get<json::OrderType>();
  auto time_in_force = [&]() -> json::TimeInForce { return map(create_order.time_in_force); }();
  auto reduce_only = [&]() { return create_order.execution_instructions.has(ExecutionInstruction::DO_NOT_INCREASE); }();
  auto price = create_order.price * security.price_factor;  // note!
  log::warn("DEBUG scaling price {} => {}"sv, create_order.price, price);
  fmt::format_to(
      std::back_inserter(buffer),
      "?clOrdID={}"
      "&symbol={}"
      "&side={}"
      "&posSide=Long"  // XXX FIXME TODO
      "&ordType={}"
      "&timeInForce={}"
      "&reduceOnly={}"
      "&orderQty={}"sv,
      request_id,
      create_order.symbol,
      side.as_raw_text(),
      ord_type.as_raw_text(),
      time_in_force.as_raw_text(),
      reduce_only,
      Decimal{create_order.quantity, order.quantity_precision.precision});
  if (!std::isnan(create_order.price)) {
    fmt::format_to(std::back_inserter(buffer), "&priceEp={}"sv, Decimal{price, Precision::_0});  // note!
  }
  if (!std::isnan(create_order.stop_price)) {
    fmt::format_to(std::back_inserter(buffer), "&stopPxEp={}"sv, Decimal{create_order.stop_price, order.price_precision.precision});
  }
  return buffer;
}

// XXX FIXME TODO stpInstruction
std::string_view Encoder::create_order_usd_m(
    std::string &buffer, CreateOrder const &create_order, server::oms::Order const &order, std::string_view const &request_id) {
  buffer.clear();
  auto side = map(create_order.side).template get<json::Side>();
  auto ord_type = map(create_order.order_type).template get<json::OrderType>();
  auto time_in_force = map(create_order.time_in_force).template get<json::TimeInForce>();
  auto reduce_only = [&]() { return create_order.execution_instructions.has(ExecutionInstruction::DO_NOT_INCREASE); }();
  fmt::format_to(
      std::back_inserter(buffer),
      "?clOrdID={}"
      "&symbol={}"
      "&side={}"
      "&posSide=Long"  // XXX FIXME TODO
      "&ordType={}"
      "&timeInForce={}"
      "&reduceOnly={}"
      "&orderQtyRq={}"sv,
      request_id,
      create_order.symbol,
      side.as_raw_text(),
      ord_type.as_raw_text(),
      time_in_force.as_raw_text(),
      reduce_only,
      Decimal{create_order.quantity, order.quantity_precision.precision});
  if (!std::isnan(create_order.price)) {
    fmt::format_to(std::back_inserter(buffer), "&priceRp={}"sv, Decimal{create_order.price, order.price_precision.precision});
  }
  if (!std::isnan(create_order.stop_price)) {
    fmt::format_to(std::back_inserter(buffer), "&stopPxRp={}"sv, Decimal{create_order.stop_price, order.price_precision.precision});
  }
  return buffer;
}

std::string_view Encoder::modify_order_coin_m(
    std::string &buffer, ModifyOrder const &modify_order, server::oms::Order const &order, [[maybe_unused]] std::string_view const &request_id) {
  buffer.clear();
  if (std::empty(order.external_order_id)) {
    fmt::format_to(std::back_inserter(buffer), "?origClOrdID={}"sv, order.client_order_id);
  } else {
    fmt::format_to(std::back_inserter(buffer), "?orderID={}"sv, order.external_order_id);
  }
  fmt::format_to(std::back_inserter(buffer), "&symbol={}"sv, order.symbol);
  if (!std::isnan(modify_order.quantity)) {
    fmt::format_to(std::back_inserter(buffer), "&orderQty={}"sv, Decimal{modify_order.quantity, order.quantity_precision.precision});
  }
  if (!std::isnan(modify_order.price)) {
    fmt::format_to(std::back_inserter(buffer), "&price={}"sv, Decimal{modify_order.price, order.price_precision.precision});
  }
  return buffer;
}

std::string_view Encoder::modify_order_usd_m(
    std::string &buffer, ModifyOrder const &modify_order, server::oms::Order const &order, [[maybe_unused]] std::string_view const &request_id) {
  buffer.clear();
  if (std::empty(order.external_order_id)) {
    fmt::format_to(std::back_inserter(buffer), "?origClOrdID={}"sv, order.client_order_id);
  } else {
    fmt::format_to(std::back_inserter(buffer), "?orderID={}"sv, order.external_order_id);
  }
  fmt::format_to(
      std::back_inserter(buffer),
      "&symbol={}"
      "&posSide=Long"sv,  // XXX FIXME TODO
      order.symbol);
  if (!std::isnan(modify_order.quantity)) {
    fmt::format_to(std::back_inserter(buffer), "&orderQtyRq={}"sv, Decimal{modify_order.quantity, order.quantity_precision.precision});
  }
  if (!std::isnan(modify_order.price)) {
    fmt::format_to(std::back_inserter(buffer), "&priceRp={}"sv, Decimal{modify_order.price, order.price_precision.precision});
  }
  return buffer;
}

std::string_view Encoder::cancel_order_coin_m(
    std::string &buffer, CancelOrder const &, server::oms::Order const &order, [[maybe_unused]] std::string_view const &request_id) {
  assert(!std::empty(symbol));
  buffer.clear();
  if (std::empty(order.external_order_id)) {
    fmt::format_to(std::back_inserter(buffer), "?clOrdID={}"sv, order.client_order_id);
  } else {
    fmt::format_to(std::back_inserter(buffer), "?orderID={}"sv, order.external_order_id);
  }
  fmt::format_to(std::back_inserter(buffer), "&symbol={}"sv, order.symbol);
  return buffer;
}

std::string_view Encoder::cancel_order_usd_m(
    std::string &buffer, CancelOrder const &, server::oms::Order const &order, [[maybe_unused]] std::string_view const &request_id) {
  assert(!std::empty(symbol));
  buffer.clear();
  if (std::empty(order.external_order_id)) {
    fmt::format_to(std::back_inserter(buffer), "?clOrdID={}"sv, order.client_order_id);
  } else {
    fmt::format_to(std::back_inserter(buffer), "?orderID={}"sv, order.external_order_id);
  }
  fmt::format_to(
      std::back_inserter(buffer),
      "&symbol={}"
      "&posSide=Long"sv,  // XXX FIXME TODO
      order.symbol);
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
