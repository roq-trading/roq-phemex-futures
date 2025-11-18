/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include <chrono>
#include <string>
#include <string_view>

#include "roq/cancel_all_orders.hpp"
#include "roq/cancel_order.hpp"
#include "roq/create_order.hpp"
#include "roq/modify_order.hpp"

#include "roq/server/oms/order.hpp"

#include "roq/phemex_futures/tools/security.hpp"

namespace roq {
namespace phemex_futures {
namespace json {

struct Encoder final {
  static std::string_view create_order_coin_m(
      std::string &buffer, CreateOrder const &, server::oms::Order const &, std::string_view const &request_id, tools::Security const &);
  static std::string_view create_order_usd_m(std::string &buffer, CreateOrder const &, server::oms::Order const &, std::string_view const &request_id);

  static std::string_view modify_order_coin_m(std::string &buffer, ModifyOrder const &, server::oms::Order const &, std::string_view const &request_id);
  static std::string_view modify_order_usd_m(std::string &buffer, ModifyOrder const &, server::oms::Order const &, std::string_view const &request_id);

  static std::string_view cancel_order_coin_m(std::string &buffer, CancelOrder const &, server::oms::Order const &, std::string_view const &request_id);
  static std::string_view cancel_order_usd_m(std::string &buffer, CancelOrder const &, server::oms::Order const &, std::string_view const &request_id);

  static std::string_view cancel_all_orders(std::string &buffer, CancelAllOrders const &, std::string_view const &symbol, std::string_view const &request_id);
};

}  // namespace json
}  // namespace phemex_futures
}  // namespace roq
