/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

using value_type = json::Market24h;

TEST_CASE("coin_m_simple", "[json_market24h]") {
  auto message = R"({)"
                 R"("market24h":{)"
                 R"("close":1094389000,)"
                 R"("fundingRate":2481,)"
                 R"("high":1097721000,)"
                 R"("indexPrice":1095022905,)"
                 R"("low":1090714000,)"
                 R"("markPrice":1094389000,)"
                 R"("open":1094937000,)"
                 R"("openInterest":2521160395,)"
                 R"("predFundingRate":2481,)"
                 R"("symbol":"BTCUSD",)"
                 R"("turnover":118157120811,)"
                 R"("volume":129318979)"
                 R"(},)"
                 R"("timestamp":1759029819812080202)"
                 R"(})";
  auto helper = [](value_type const &obj) { CHECK(obj.timestamp == 1759029819812080202ns); };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}
