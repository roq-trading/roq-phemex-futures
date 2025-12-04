/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

using value_type = json::AccountsOrdersPositions;

TEST_CASE("coin_m_simple", "[json_accounts_orders_positions]") {
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
  auto helper = [](value_type const &obj) { CHECK(obj.sequence == 639401975); };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}

TEST_CASE("coin_m_orders", "[json_accounts_orders_positions]") {
  auto message = R"({)"
                 R"("sequence":641746873,)"
                 R"("timestamp":1763440759389104504,)"
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
                 R"("orders":[{)"
                 R"("accountID":88606880001,)"
                 R"("action":"New",)"
                 R"("actionBy":"ByUser",)"
                 R"("actionTimeNs":1763438985930737532,)"
                 R"("addedSeq":0,)"
                 R"("bonusChangedAmountEv":0,)"
                 R"("clOrdID":"IQACYzbrm0AAAQAAAAAA",)"
                 R"("closedPnlEv":0,)"
                 R"("closedSize":0,)"
                 R"("code":11058,)"
                 R"("cumQty":0,)"
                 R"("cumValueEv":0,)"
                 R"("curAccBalanceEv":50649,)"
                 R"("curAssignedPosBalanceEv":0,)"
                 R"("curBonusBalanceEv":0,)"
                 R"("curLeverageEr":-1000000000,)"
                 R"("curPosSide":"None",)"
                 R"("curPosSize":0,)"
                 R"("curPosTerm":1,)"
                 R"("curPosValueEv":0,)"
                 R"("currency":"BTC",)"
                 R"("cxlRejReason":0,)"
                 R"("displayQty":0,)"
                 R"("execFeeEv":0,)"
                 R"("execID":"00000000-0000-0000-0000-000000000000",)"
                 R"("execPriceEp":0,)"
                 R"("execQty":0,)"
                 R"("execSeq":0,)"
                 R"("execStatus":"CreateRejected",)"
                 R"("execValueEv":0,)"
                 R"("feeRateEr":0,)"
                 R"("leavesQty":0,)"
                 R"("leavesValueEv":0,)"
                 R"("message":"",)"
                 R"("ordStatus":"Rejected",)"
                 R"("ordType":"Limit",)"
                 R"("orderID":"9787c128-b464-4cae-9296-fd78e1c4a4f4",)"
                 R"("orderQty":0,)"
                 R"("pegOffsetValueEp":0,)"
                 R"("priceEp":0,)"
                 R"("relatedPosTerm":1,)"
                 R"("relatedReqNum":3,)"
                 R"("side":"Buy",)"
                 R"("slPxEp":0,)"
                 R"("slTrigger":"ByMarkPrice",)"
                 R"("slippageEr":0,)"
                 R"("stopLossEp":0,)"
                 R"("stopPxEp":0,)"
                 R"("symbol":"BTCUSD",)"
                 R"("takeProfitEp":0,)"
                 R"("timeInForce":"GoodTillCancel",)"
                 R"("tpPxEp":0,)"
                 R"("tpTrigger":"ByLastPrice",)"
                 R"("transactTimeNs":1763438985930737532,)"
                 R"("userID":8860688)"
                 R"(})"
                 R"(],)"
                 R"("positions":[])"
                 R"(})";
  auto helper = [](value_type const &obj) { CHECK(obj.sequence == 641746873); };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}
