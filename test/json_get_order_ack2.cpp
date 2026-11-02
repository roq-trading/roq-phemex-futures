/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/phemex_futures/protocol/json/get_order_ack2.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;

using value_type = protocol::json::GetOrderAck2;

TEST_CASE("usd_m_simple", "[json_get_order_ack]") {
  auto message = R"({)"
                 R"("code":0,)"
                 R"("msg":"OK",)"
                 R"("data":{)"
                 R"("rows":[{)"
                 R"("orderId":"123f5d80-2d1e-4e73-a9b8-82909163d93a",)"
                 R"("clOrdId":"1wgCAF6S0YYAAQAAAAAA",)"
                 R"("symbol":"BTCUSDT",)"
                 R"("side":"Buy",)"
                 R"("ordType":"Limit",)"
                 R"("actionTimeNs":1793593771716684638,)"
                 R"("priceRp":"50",)"
                 R"("orderQtyRq":"1",)"
                 R"("displayQtyRq":"0",)"
                 R"("timeInForce":"GoodTillCancel",)"
                 R"("reduceOnly":false,)"
                 R"("takeProfitRp":"0",)"
                 R"("stopLossRp":"0",)"
                 R"("closedPnlRv":"0",)"
                 R"("closedSizeRq":"0",)"
                 R"("cumQtyRq":"0",)"
                 R"("cumValueRv":"0",)"
                 R"("leavesQtyRq":"0",)"
                 R"("leavesValueRv":"0",)"
                 R"("stopDirection":"UNSPECIFIED",)"
                 R"("ordStatus":"Rejected",)"
                 R"("transactTimeNs":1793593771716684638,)"
                 R"("bizError":11055)"
                 R"(})"
                 R"(])"
                 R"(})"
                 R"(})"sv;
  auto helper = [&](value_type const &obj) { CHECK(obj.code == 0); };
  core::json::BufferStack buffers{4096, 2};
  value_type obj{message, buffers};
  helper(obj);
}
