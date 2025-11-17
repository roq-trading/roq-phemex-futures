/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include <string_view>

#include "roq/phemex_futures/settings.hpp"

namespace roq {
namespace phemex_futures {

struct API final {
  enum class Type {
    COIN_M,
    USD_M,
  };

  Type type;
  struct {
    std::string_view products;
    std::string_view orderbook;
    std::string_view trade;
    std::string_view market24h;
    std::string_view kline;
  } market_data;

  struct {
    std::string_view accounts_orders_positions;
    std::string_view account_info;
    std::string_view account_assets;
    std::string_view position_info;
    std::string_view open_orders;
    std::string_view fill_history;
    std::string_view place_order;
    std::string_view modify_order;
    std::string_view cancel_order;
    std::string_view cancel_all_orders;
    std::string_view countdown_cancel_all;
  } order_management;

  // factory
  static API create(Settings const &);
};

}  // namespace phemex_futures
}  // namespace roq
