/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/phemex/json/accounts_orders_positions.hpp"

using namespace roq;
using namespace roq::phemex;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

TEST_CASE("simple", "[json_accounts_orders_positions]") {
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
