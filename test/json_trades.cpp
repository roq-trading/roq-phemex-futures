/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;

using value_type = json::Trades;

// note! truncated
TEST_CASE("simple", "[json_trades]") {
  auto message = R"({)"
                 R"("sequence":20254182892,)"
                 R"("symbol":"BTCUSD",)"
                 R"("trades":[)"
                 R"([1758962421019961472,"Sell",1093078000,1],)"
                 R"([1758962421019961472,"Sell",1093110000,1],)"
                 R"([1758950929672637218,"Buy",1095929000,201],)"
                 R"([1758950929672637218,"Buy",1095929000,201])"
                 R"(],)"
                 R"("type":"snapshot")"
                 R"(})";
  auto helper = [](value_type const &obj) { CHECK(obj.sequence == 20254182892); };
  ParserTester<value_type>::dispatch(helper, message, 8192, 2);
}
