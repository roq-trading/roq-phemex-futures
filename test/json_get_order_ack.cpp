/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/phemex_futures/protocol/json/get_order_ack.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;

using value_type = protocol::json::GetOrderAck;

TEST_CASE("coin_m_simple", "[json_get_order_ack]") {
  auto message = R"({)"
                 R"("code":0,)"
                 R"("msg":"OK",)"
                 R"("data":[{)"
                 R"("orderID":"c1eb9b86-570f-4292-97e5-354b278013bc",)"
                 R"("clOrdID":"QwQCxTmr0oYAAQAAAAAA",)"
                 R"("symbol":"BTCUSD",)"
                 R"("side":"Buy",)"
                 R"("orderType":"Limit",)"
                 R"("actionTimeNs":1793595624403211703,)"
                 R"("priceEp":500000,)"
                 R"("price":null,)"
                 R"("orderQty":1,)"
                 R"("displayQty":0,)"
                 R"("timeInForce":"GoodTillCancel",)"
                 R"("reduceOnly":false,)"
                 R"("takeProfitEp":0,)"
                 R"("takeProfit":null,)"
                 R"("stopLossEp":0,)"
                 R"("closedPnlEv":0,)"
                 R"("closedPnl":null,)"
                 R"("closedSize":0,)"
                 R"("cumQty":0,)"
                 R"("cumValueEv":0,)"
                 R"("cumValue":null,)"
                 R"("leavesQty":0,)"
                 R"("leavesValueEv":0,)"
                 R"("leavesValue":null,)"
                 R"("stopLoss":null,)"
                 R"("stopDirection":"UNSPECIFIED",)"
                 R"("ordStatus":"Rejected",)"
                 R"("transactTimeNs":1793595624403211703,)"
                 R"("bizError":11055)"
                 R"(})"
                 R"(])"
                 R"(})"sv;
  auto helper = [&](value_type const &obj) { CHECK(obj.code == 0); };
  core::json::BufferStack buffers{4096, 2};
  value_type obj{message, buffers};
  helper(obj);
}
