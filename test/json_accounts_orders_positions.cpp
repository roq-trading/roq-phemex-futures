/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/phemex_futures/json/accounts_orders_positions.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

TEST_CASE("simple_coin_m", "[json_accounts_orders_positions]") {
  auto message = R"({)"
                 R"("sequence":639401975,)"
                 R"("timestamp":1763353202363975403,)"
                 R"("type":"snapshot",)"
                 R"("version":0,)"
                 R"("accounts":[{)"
                 R"("accountBalanceEv":50649,)"
                 R"("accountID":88606880001,)"
                 R"("bonusBalanceEv":0,)"
                 R"("currency":"BTC",)"
                 R"("liqStatus":"Normal",)"
                 R"("marginRatioEr":99900000000,)"
                 R"("status":1,)"
                 R"("totalUsedBalanceEv":0,)"
                 R"("userID":8860688,)"
                 R"("userMode":"Classic",)"
                 R"("ver":1)"
                 R"(})"
                 R"(],)"
                 R"("orders":[],)"
                 R"("positions":[])"
                 R"(})";
  core::json::BufferStack buffer{8192, 1};
  json::AccountsOrdersPositions obj{message, buffer};
}

TEST_CASE("simple_usd_m", "[json_accounts_orders_positions]") {
  auto message = R"({)"
                 R"("sequence":1755327186,)"
                 R"("timestamp":1763362821375642795,)"
                 R"("type":"snapshot",)"
                 R"("version":0,)"
                 R"("accounts_p":[{)"
                 R"("accountBalanceRv":"125.0635927",)"
                 R"("accountID":88606880003,)"
                 R"("bonusBalanceRv":"0",)"
                 R"("currency":"USDT",)"
                 R"("status":0,)"
                 R"("totalUsedBalanceRv":"0",)"
                 R"("tradeLevel":0,)"
                 R"("userID":8860688,)"
                 R"("userMode":"Classic")"
                 R"(})"
                 R"(],)"
                 R"("orders_p":[],)"
                 R"("positions_p":[])"
                 R"(})";
  core::json::BufferStack buffer{8192, 1};
  json::AccountsOrdersPositions obj{message, buffer};
}
