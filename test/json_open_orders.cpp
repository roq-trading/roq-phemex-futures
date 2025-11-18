/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/phemex_futures/json/open_orders.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;

TEST_CASE("coin_m_simple", "[json_open_orders]") {
  auto message = R"({)"
                 R"("code":0,)"
                 R"("msg":"",)"
                 R"("data":{)"
                 R"("rows":[{)"
                 R"("bizError":0,)"
                 R"("orderID":"27ab3d87-b98c-458d-9f8e-5bf1086f7b2b",)"
                 R"("clOrdID":"kwACHaeEqUAAAQAAAAAA",)"
                 R"("symbol":"BTCUSD",)"
                 R"("side":"Buy",)"
                 R"("actionTimeNs":1763461802766214046,)"
                 R"("transactTimeNs":1763461802769939425,)"
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
                 R"("execStatus":"New",)"
                 R"("pegPriceType":"UNSPECIFIED",)"
                 R"("ordStatus":"New",)"
                 R"("execInst":"None")"
                 R"(})"
                 R"(],)"
                 R"("nextPageArg":"")"
                 R"(})"
                 R"(})"sv;
  core::json::BufferStack buffers{65536, 2};
  [[maybe_unused]] json::OpenOrders obj{message, buffers};
}
