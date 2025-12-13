/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;

using value_type = json::Kline;

// note! truncated
TEST_CASE("snapshot", "[json_kline]") {
  auto message = R"({)"
                 R"("kline":[)"
                 R"([1759040340,60,1093523000,1093522000,1093523000,1093522000,1093522000,94584,86494822],)"
                 R"([1759040280,60,1093522000,1093523000,1093523000,1093523000,1093523000,248,226789],)"
                 R"([1758920460,60,1092502000,1092503000,1092503000,1092503000,1092503000,1,915],)"
                 R"([1758920400,60,1093174000,1093111000,1093111000,1092502000,1092502000,106,96972])"
                 R"(],)"
                 R"("priceScale":4,)"
                 R"("sequence":20256660818,)"
                 R"("symbol":"BTCUSD",)"
                 R"("type":"snapshot",)"
                 R"("valueScale":8)"
                 R"(})"
                 R"(})";
  auto helper = [](value_type const &obj) { CHECK(obj.type == json::MessageType::SNAPSHOT); };
  ParserTester<value_type>::dispatch(helper, message, 8192, 2);
}

TEST_CASE("incremental", "[json_kline]") {
  auto message = R"({)"
                 R"("kline":[)"
                 R"([1759040820,60,6459000,6468000,6483000,6468000,6483000,269388700000,17438876605])"
                 R"(],)"
                 R"("priceScale":8,)"
                 R"("qtyScale":8,)"
                 R"("sequence":18268599368,)"
                 R"("symbol":"sPUMPBTCUSDT",)"
                 R"("type":"incremental",)"
                 R"("valueScale":8)"
                 R"(})";
  auto helper = [](value_type const &obj) { CHECK(obj.type == json::MessageType::INCREMENTAL); };
  ParserTester<value_type>::dispatch(helper, message, 8192, 2);
}
