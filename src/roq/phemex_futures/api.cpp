/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/phemex_futures/api.hpp"

#include "roq/logging.hpp"

#include "roq/utils/enum.hpp"

using namespace std::literals;

namespace roq {
namespace phemex_futures {

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
        .account_info = "/api/v3/account/settings"sv,
        .account_assets = "/api/v3/account/assets"sv,
        .position_info = "/api/v3/position/current-position"sv,
        .open_orders = "/api/v3/trade/unfilled-orders"sv,
        .fill_history = "/api/v3/trade/fills"sv,
        .place_order = "/api/v3/trade/place-order"sv,
        .modify_order = "/api/v3/trade/modify-order"sv,
        .cancel_order = "/api/v3/trade/cancel-order"sv,
        .cancel_all_orders = "/api/v3/trade/cancel-symbol-order"sv,
        .countdown_cancel_all = "/api/v3/trade/countdown-cancel-all"sv,
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
        .account_info = "/api/v3/account/settings"sv,
        .account_assets = "/api/v3/account/assets"sv,
        .position_info = "/api/v3/position/current-position"sv,
        .open_orders = "/api/v3/trade/unfilled-orders"sv,
        .fill_history = "/api/v3/trade/fills"sv,
        .place_order = "/api/v3/trade/place-order"sv,
        .modify_order = "/api/v3/trade/modify-order"sv,
        .cancel_order = "/api/v3/trade/cancel-order"sv,
        .cancel_all_orders = "/api/v3/trade/cancel-symbol-order"sv,
        .countdown_cancel_all = "/api/v3/trade/countdown-cancel-all"sv,
    },
};
}  // namespace

// === HELPERS ===

namespace {
auto parse_api(auto &api) {
  std::string tmp{api};
  std::replace(tmp.begin(), tmp.end(), '-', '_');
  return utils::parse_enum<API::Type>(tmp);
}
}  // namespace

// === IMPLEMENTATION ===

API API::create(Settings const &settings) {
  auto key = parse_api(settings.api);
  switch (key) {
    using enum Type;
    case COIN_M:
      return API_COIN_M;
    case USD_M:
      return API_USD_M;
  }
  log::fatal("Unexpected"sv);
}

}  // namespace phemex_futures
}  // namespace roq
