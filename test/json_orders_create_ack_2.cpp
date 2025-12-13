/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/phemex_futures/json/orders_create_ack2.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;

using value_type = json::OrdersCreateAck2;

TEST_CASE("usd_m_simple", "[json_orders_create_ack2]") {
  auto message = R"({)"
                 R"("code":0,)"
                 R"("msg":"",)"
                 R"("data":{)"
                 R"("bizError":0,)"
                 R"("orderID":"b5760445-d6c9-489a-805d-669a4ea9ead6",)"
                 R"("clOrdID":"EAACDVL8t0AAAQAAAAAA",)"
                 R"("symbol":"APTUSDT",)"
                 R"("side":"Buy",)"
                 R"("actionTimeNs":1763486096357633582,)"
                 R"("transactTimeNs":1763486096357633582,)"
                 R"("orderType":"Limit",)"
                 R"("priceRp":"1",)"
                 R"("orderQtyRq":"1",)"
                 R"("displayQtyRq":"0",)"
                 R"("timeInForce":"GoodTillCancel",)"
                 R"("closedPnlRv":"0",)"
                 R"("closedSizeRq":"0",)"
                 R"("cumQtyRq":"0",)"
                 R"("cumValueRv":"0",)"
                 R"("leavesQtyRq":"1",)"
                 R"("leavesValueRv":"1",)"
                 R"("stopDirection":"UNSPECIFIED",)"
                 R"("stopPxRp":"0",)"
                 R"("trigger":"UNSPECIFIED",)"
                 R"("pegOffsetValueRp":"0",)"
                 R"("pegOffsetProportionRr":"0",)"
                 R"("execStatus":"PendingNew",)"
                 R"("pegPriceType":"UNSPECIFIED",)"
                 R"("ordStatus":"Created",)"
                 R"("execInst":"None",)"
                 R"("takeProfitRp":"0",)"
                 R"("stopLossRp":"0",)"
                 R"("slPxRp":"0",)"
                 R"("tpPxRp":"0")"
                 R"(})"
                 R"(})"sv;
  auto helper = [&](value_type const &obj) { CHECK(obj.code == 0); };
  value_type obj{message};
  helper(obj);
}
