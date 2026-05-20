/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/phemex_futures/gateway/api.hpp"

#include "roq/logging.hpp"

#include "roq/utils/enum.hpp"

using namespace std::literals;

namespace roq {
namespace phemex_futures {
namespace gateway {

// === CONSTANTS ===

namespace {
auto const API_COIN_M = API{
    .type = API::Type::COIN_M,
    .market_data{
        .products = "/public/products",
        .orderbook = "orderbook"sv,
        .trade = "trade"sv,
        .market24h = "market24h"sv,
        .kline = "kline"sv,
    },
    .order_management{
        .accounts_orders_positions = "aop"sv,
        .open_orders = "/orders/activeList"sv,
        .fill_history = "/exchange/order/trade"sv,
        .orders_create = "/orders/create"sv,
        .orders_replace = "/orders/replace"sv,
        .orders_cancel = "/orders/cancel"sv,
        .orders_all = "/orders/all"sv,
    },
};
auto const API_USD_M = API{
    .type = API::Type::USD_M,
    .market_data{
        .products = "/public/products",
        .orderbook = "orderbook_p"sv,
        .trade = "trade_p"sv,
        .market24h = "market24h_p"sv,
        .kline = "kline_p"sv,
    },
    .order_management{
        .accounts_orders_positions = "aop_p"sv,
        .open_orders = "/g-orders/activeList"sv,
        .fill_history = "/exchange/order/v2/tradingList"sv,
        .orders_create = "/g-orders/create"sv,
        .orders_replace = "/g-orders/replace"sv,
        .orders_cancel = "/g-orders/cancel"sv,
        .orders_all = "/g-orders/all"sv,
    },
};
}  // namespace

// === IMPLEMENTATION ===

API API::create(Settings const &settings) {
  auto key = parse_api(settings);
  switch (key) {
    using enum Type;
    case COIN_M:
      return API_COIN_M;
    case USD_M:
      return API_USD_M;
  }
  log::fatal("Unexpected"sv);
}

API::Type API::parse_api(Settings const &settings) {
  std::string tmp{settings.api};
  std::replace(tmp.begin(), tmp.end(), '-', '_');
  return utils::parse_enum<API::Type>(tmp);
}

}  // namespace gateway
}  // namespace phemex_futures
}  // namespace roq
