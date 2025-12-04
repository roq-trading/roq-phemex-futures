/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_2_tester.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

using value_type = json::Market24h2;

TEST_CASE("usd_m_simple", "[json_market24h_2]") {
  auto message = R"({)"
                 R"("market24h_p":{)"
                 R"("closeRp":"95466",)"
                 R"("fundingRateRr":"-0.00000599",)"
                 R"("highRp":"95922.3",)"
                 R"("indexPriceRp":"95510.92042366",)"
                 R"("lowRp":"92860.3",)"
                 R"("markPriceRp":"95472.2",)"
                 R"("openInterestRv":"96.3639",)"
                 R"("openRp":"95498.6",)"
                 R"("predFundingRateRr":"-0.00000599",)"
                 R"("symbol":"BTCUSDC",)"
                 R"("turnoverRv":"112081258.89864",)"
                 R"("volumeRq":"1183.2525")"
                 R"(},)"
                 R"("timestamp":1763380521025768914)"
                 R"(})";
  auto helper = [](value_type const &obj) { CHECK(obj.timestamp == 1763380521025768914ns); };
  Parser2Tester<value_type>::dispatch(helper, message, 8192, 1);
}
