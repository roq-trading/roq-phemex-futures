/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_2_tester.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

using value_type = json::AccountsOrdersPositions2;

TEST_CASE("usd_m_simple", "[json_accounts_orders_positions]") {
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
  auto helper = [](value_type const &obj) { CHECK(obj.sequence == 1755327186); };
  Parser2Tester<value_type>::dispatch(helper, message, 8192, 1);
}

TEST_CASE("usd_m_orders", "[json_accounts_orders_positions]") {
  auto message = R"({)"
                 R"("sequence":1773392251,)"
                 R"("timestamp":1763453447020323318,)"
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
                 R"("orders_p":[{)"
                 R"("accountID":88606880003,)"
                 R"("action":"New",)"
                 R"("actionBy":"ByUser",)"
                 R"("actionTimeNs":1763445051665408719,)"
                 R"("addedSeq":0,)"
                 R"("bonusChangedAmountRv":"0",)"
                 R"("clOrdID":"dAACj-iIn0AAAQAAAAAA",)"
                 R"("cl_req_code":0,)"
                 R"("closedPnlRv":"0",)"
                 R"("closedSize":"0",)"
                 R"("code":11001,)"
                 R"("cumFeeRv":"0",)"
                 R"("cumPtFeeRv":"0",)"
                 R"("cumQty":"0",)"
                 R"("cumValueRv":"0",)"
                 R"("curAccBalanceRv":"125.0635927",)"
                 R"("curAssignedPosBalanceRv":"0",)"
                 R"("curBonusBalanceRv":"0",)"
                 R"("curLeverageRr":"-10",)"
                 R"("curPosSide":"None",)"
                 R"("curPosSize":"0",)"
                 R"("curPosTerm":1,)"
                 R"("curPosValueRv":"0",)"
                 R"("curRiskLimitRv":"20000000",)"
                 R"("currency":"USDT",)"
                 R"("cxlRejReason":0,)"
                 R"("displayQty":"0",)"
                 R"("execFeeRv":"0",)"
                 R"("execID":"00000000-0000-0000-0000-000000000000",)"
                 R"("execPriceRp":"0",)"
                 R"("execQty":"0",)"
                 R"("execSeq":0,)"
                 R"("execStatus":"CreateRejected",)"
                 R"("execValueRv":"0",)"
                 R"("feeRateRr":"0",)"
                 R"("leavesQty":"1",)"
                 R"("leavesValueRv":"32000",)"
                 R"("message":"TE: no enough available balance",)"
                 R"("ordStatus":"Rejected",)"
                 R"("ordType":"Limit",)"
                 R"("orderID":"459f2c3a-ac61-400b-8e7f-915e88c4e253",)"
                 R"("orderQty":"1",)"
                 R"("pegOffsetProportionRr":"0",)"
                 R"("pegOffsetValueRp":"0",)"
                 R"("posSide":"Long",)"
                 R"("posTpSlPattern":"None",)"
                 R"("priceRp":"32000",)"
                 R"("ptFeeRv":"0",)"
                 R"("ptPriceRp":"0",)"
                 R"("relatedPosTerm":1,)"
                 R"("relatedReqNum":3,)"
                 R"("side":"Buy",)"
                 R"("slPxRp":"0",)"
                 R"("slTimeInForce":"ImmediateOrCancel",)"
                 R"("slTrigger":"ByMarkPrice",)"
                 R"("stopLossRp":"0",)"
                 R"("stopPxRp":"0",)"
                 R"("symbol":"BTCUSDT",)"
                 R"("takeProfitRp":"0",)"
                 R"("timeInForce":"GoodTillCancel",)"
                 R"("tpPxRp":"0",)"
                 R"("tpTimeInForce":"ImmediateOrCancel",)"
                 R"("tpTrigger":"ByLastPrice",)"
                 R"("transactTimeNs":1763445051665408719,)"
                 R"("userID":8860688)"
                 R"(})"
                 R"(],)"
                 R"("positions_p":[])"
                 R"(})";
  auto helper = [](value_type const &obj) { CHECK(obj.sequence == 1773392251); };
  Parser2Tester<value_type>::dispatch(helper, message, 8192, 1);
}
