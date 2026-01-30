/* Copyright (c) 2017-2026, Hans Erik Thrane */

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
    std::string_view open_orders;
    std::string_view fill_history;
    std::string_view orders_create;
    std::string_view orders_replace;
    std::string_view orders_cancel;
    std::string_view orders_all;
  } order_management;

  // factory
  static API create(Settings const &);

  Type static parse_api(Settings const &);
};

}  // namespace phemex_futures
}  // namespace roq
