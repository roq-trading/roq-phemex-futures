/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/phemex_futures/json/parser.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

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
  core::json::BufferStack buffers{8192, 1};
  // simple
  json::AccountsOrdersPositions obj{message, buffers};
  CHECK(obj.sequence == 639401975);
  // parser
  struct Handler final : public json::Parser::Handler {
    void operator()(Trace<json::Pong> const &) override { FAIL(); }
    void operator()(Trace<json::Ack> const &) override { FAIL(); }
    void operator()(Trace<json::Book> const &) override { FAIL(); }
    void operator()(Trace<json::Trades> const &) override { FAIL(); }
    void operator()(Trace<json::Market24h> const &) override { FAIL(); }
    void operator()(Trace<json::Market24h2> const &) override { FAIL(); }
    void operator()(Trace<json::Kline> const &) override { FAIL(); }
    void operator()(Trace<json::IndexMarket24h> const &) override { FAIL(); }
    void operator()(Trace<json::AccountsOrdersPositions> const &event) override {
      found = true;
      auto &[trace_info, accounts_orders_positions] = event;
      CHECK(accounts_orders_positions.sequence == 639401975);
    }
    void operator()(Trace<json::AccountsOrdersPositions2> const &) override { FAIL(); }
    void operator()(Trace<json::PositionInfo> const &) override { FAIL(); }

    bool found = false;
  } handler;
  auto res = json::Parser::dispatch(handler, message, buffers, {}, false);
  CHECK(res == true);
  CHECK(handler.found == true);
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
  core::json::BufferStack buffers{8192, 1};
  // simple
  json::AccountsOrdersPositions obj{message, buffers};
  CHECK(obj.sequence == 641746873);
  // parser
  struct Handler final : public json::Parser::Handler {
    void operator()(Trace<json::Pong> const &) override { FAIL(); }
    void operator()(Trace<json::Ack> const &) override { FAIL(); }
    void operator()(Trace<json::Book> const &) override { FAIL(); }
    void operator()(Trace<json::Trades> const &) override { FAIL(); }
    void operator()(Trace<json::Market24h> const &) override { FAIL(); }
    void operator()(Trace<json::Market24h2> const &) override { FAIL(); }
    void operator()(Trace<json::Kline> const &) override { FAIL(); }
    void operator()(Trace<json::IndexMarket24h> const &) override { FAIL(); }
    void operator()(Trace<json::AccountsOrdersPositions> const &event) override {
      found = true;
      auto &[trace_info, accounts_orders_positions] = event;
      CHECK(accounts_orders_positions.sequence == 641746873);
    }
    void operator()(Trace<json::AccountsOrdersPositions2> const &) override { FAIL(); }
    void operator()(Trace<json::PositionInfo> const &) override { FAIL(); }

    bool found = false;
  } handler;
  auto res = json::Parser::dispatch(handler, message, buffers, {}, false);
  CHECK(res == true);
  CHECK(handler.found == true);
}

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
  core::json::BufferStack buffers{8192, 1};
  // simple
  json::AccountsOrdersPositions2 obj{message, buffers};
  CHECK(obj.sequence == 1755327186);
  // parser
  struct Handler final : public json::Parser::Handler {
    void operator()(Trace<json::Pong> const &) override { FAIL(); }
    void operator()(Trace<json::Ack> const &) override { FAIL(); }
    void operator()(Trace<json::Book> const &) override { FAIL(); }
    void operator()(Trace<json::Trades> const &) override { FAIL(); }
    void operator()(Trace<json::Market24h> const &) override { FAIL(); }
    void operator()(Trace<json::Market24h2> const &) override { FAIL(); }
    void operator()(Trace<json::Kline> const &) override { FAIL(); }
    void operator()(Trace<json::IndexMarket24h> const &) override { FAIL(); }
    void operator()(Trace<json::AccountsOrdersPositions> const &) override { FAIL(); }
    void operator()(Trace<json::AccountsOrdersPositions2> const &event) override {
      found = true;
      auto &[trace_info, accounts_orders_positions] = event;
      CHECK(accounts_orders_positions.sequence == 1755327186);
    }

    void operator()(Trace<json::PositionInfo> const &) override { FAIL(); }

    bool found = false;
  } handler;
  auto res = json::Parser::dispatch(handler, message, buffers, {}, false);
  CHECK(res == true);
  CHECK(handler.found == true);
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
  core::json::BufferStack buffers{8192, 1};
  // simple
  json::AccountsOrdersPositions2 obj{message, buffers};
  CHECK(obj.sequence == 1773392251);
  // parser
  struct Handler final : public json::Parser::Handler {
    void operator()(Trace<json::Pong> const &) override { FAIL(); }
    void operator()(Trace<json::Ack> const &) override { FAIL(); }
    void operator()(Trace<json::Book> const &) override { FAIL(); }
    void operator()(Trace<json::Trades> const &) override { FAIL(); }
    void operator()(Trace<json::Market24h> const &) override { FAIL(); }
    void operator()(Trace<json::Market24h2> const &) override { FAIL(); }
    void operator()(Trace<json::Kline> const &) override { FAIL(); }
    void operator()(Trace<json::IndexMarket24h> const &) override { FAIL(); }
    void operator()(Trace<json::AccountsOrdersPositions> const &) override { FAIL(); }
    void operator()(Trace<json::AccountsOrdersPositions2> const &event) override {
      found = true;
      auto &[trace_info, accounts_orders_positions] = event;
      CHECK(accounts_orders_positions.sequence == 1773392251);
    }

    void operator()(Trace<json::PositionInfo> const &) override { FAIL(); }

    bool found = false;
  } handler;
  auto res = json::Parser::dispatch(handler, message, buffers, {}, false);
  CHECK(res == true);
  CHECK(handler.found == true);
}
