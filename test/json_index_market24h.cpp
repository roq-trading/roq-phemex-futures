/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

using value_type = json::IndexMarket24h;

TEST_CASE("simple", "[json_index_market24h]") {
  auto message = R"({)"
                 R"("index_market24h":{)"
                 R"("highEp":9996,)"
                 R"("lastEp":9992,)"
                 R"("lowEp":9984,)"
                 R"("openEp":9993,)"
                 R"("symbol":".USDT")"
                 R"(},)"
                 R"("timestamp":1763353202275046666)"
                 R"(})";
  auto helper = [](value_type const &obj) { CHECK(obj.timestamp == 1763353202275046666ns); };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}
