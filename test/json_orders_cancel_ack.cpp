/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/phemex_futures/json/orders_cancel_ack.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;

using value_type = json::OrdersCancelAck;

TEST_CASE("coin_m_simple", "[json_orders_cancel_ack]") {
  auto message = R"({)"
                 R"("code":0,)"
                 R"("msg":"",)"
                 R"("data":{)"
                 R"("bizError":0,)"
                 R"("orderID":"e878d69c-b534-4590-8048-113d133f8016",)"
                 R"("clOrdID":"OAACYj3XtEAAAQAAAAAA",)"
                 R"("symbol":"BTCUSD",)"
                 R"("side":"Buy",)"
                 R"("actionTimeNs":1763480819161707456,)"
                 R"("transactTimeNs":1763480819161707456,)"
                 R"("orderType":"Limit",)"
                 R"("priceEp":320000000,)"
                 R"("price":32000.00000000,)"
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
                 R"("leavesValueEv":3125,)"
                 R"("leavesValue":0.00003125,)"
                 R"("stopDirection":"UNSPECIFIED",)"
                 R"("stopPxEp":0,)"
                 R"("stopPx":0E-8,)"
                 R"("trigger":"UNSPECIFIED",)"
                 R"("pegOffsetValueEp":0,)"
                 R"("pegOffsetProportionEr":0,)"
                 R"("execStatus":"PendingCancel",)"
                 R"("pegPriceType":"UNSPECIFIED",)"
                 R"("ordStatus":"New",)"
                 R"("execInst":"None")"
                 R"(})"
                 R"(})"sv;
  auto helper = [&](value_type const &obj) { CHECK(obj.code == 0); };
  value_type obj{message};
  helper(obj);
}
