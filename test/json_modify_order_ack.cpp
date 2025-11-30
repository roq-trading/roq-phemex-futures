/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/phemex_futures/json/modify_order_ack.hpp"
#include "roq/phemex_futures/json/modify_order_ack2.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;

TEST_CASE("coin_m_simple", "[json_modify_order_ack]") {
  auto message = R"({)"
                 R"("code":0,)"
                 R"("msg":"",)"
                 R"("data":{)"
                 R"("bizError":0,)"
                 R"("orderID":"3eee2ff0-c8b2-45e8-a671-5a315c8b3660",)"
                 R"("clOrdID":"wQACnwRfAEEAAQAAAAAA",)"
                 R"("symbol":"BTCUSD",)"
                 R"("side":"Buy",)"
                 R"("actionTimeNs":1763607691007511188,)"
                 R"("transactTimeNs":1763607691007511188,)"
                 R"("orderType":"Limit",)"
                 R"("priceEp":323230000,)"
                 R"("price":32323.00000000,)"
                 R"("orderQty":1,)"
                 R"("displayQty":0,)"
                 R"("timeInForce":"GoodTillCancel",)"
                 R"("reduceOnly":false,)"
                 R"("closedPnlEv":0,)"
                 R"("closedPnl":0E-8,)"
                 R"("closedSize":0,)"
                 R"("cumQty":0,)"
                 R"("cumValueEv":0,)"
                 R"("cumValue":0E-8,)"
                 R"("leavesQty":1,)"
                 R"("leavesValueEv":3093,)"
                 R"("leavesValue":0.00003093,)"
                 R"("stopDirection":"UNSPECIFIED",)"
                 R"("stopPxEp":0,)"
                 R"("stopPx":0E-8,)"
                 R"("trigger":"UNSPECIFIED",)"
                 R"("pegOffsetValueEp":0,)"
                 R"("pegOffsetProportionEr":0,)"
                 R"("execStatus":"PendingReplace",)"
                 R"("pegPriceType":"UNSPECIFIED",)"
                 R"("ordStatus":"New",)"
                 R"("execInst":"None")"
                 R"(})"
                 R"(})"sv;
  json::ModifyOrderAck obj{message};
  CHECK(obj.code == 0);
}

TEST_CASE("usd_m_simple", "[json_modify_order_ack]") {
  auto message = R"({)"
                 R"("code":0,)"
                 R"("msg":"",)"
                 R"("data":{)"
                 R"("bizError":0,)"
                 R"("orderID":"d2aa3482-d8ec-4aef-a7a7-883c14c2c26b",)"
                 R"("clOrdID":"LgACe8lD20AAAQAAAAAA",)"
                 R"("symbol":"APTUSDT",)"
                 R"("side":"Buy",)"
                 R"("actionTimeNs":1763545275458163275,)"
                 R"("transactTimeNs":1763545275458163275,)"
                 R"("orderType":"Limit",)"
                 R"("priceRp":"1.1",)"
                 R"("orderQtyRq":"1",)"
                 R"("displayQtyRq":"0",)"
                 R"("timeInForce":"GoodTillModify",)"
                 R"("closedPnlRv":"0",)"
                 R"("closedSizeRq":"0",)"
                 R"("cumQtyRq":"0",)"
                 R"("cumValueRv":"0",)"
                 R"("leavesQtyRq":"1",)"
                 R"("leavesValueRv":"1.1",)"
                 R"("stopDirection":"UNSPECIFIED",)"
                 R"("stopPxRp":"0",)"
                 R"("trigger":"UNSPECIFIED",)"
                 R"("pegOffsetValueRp":"0",)"
                 R"("pegOffsetProportionRr":"0",)"
                 R"("execStatus":"PendingReplace",)"
                 R"("pegPriceType":"UNSPECIFIED",)"
                 R"("ordStatus":"New",)"
                 R"("execInst":"None",)"
                 R"("takeProfitRp":"0",)"
                 R"("stopLossRp":"0",)"
                 R"("slPxRp":"0",)"
                 R"("tpPxRp":"0")"
                 R"(})"
                 R"(})"sv;
  json::ModifyOrderAck2 obj{message};
  CHECK(obj.code == 0);
}
