/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;

using value_type = json::PositionInfo;

TEST_CASE("simple", "[json_position_info]") {
  auto message = R"({)"
                 R"("position_info":{)"
                 R"("accountID":88606880001,)"
                 R"("light":5,)"
                 R"("symbol":"BTCUSD",)"
                 R"("userID":8860688)"
                 R"(},)"
                 R"("sequence":148153491)"
                 R"(})"sv;
  auto helper = [](value_type const &obj) { CHECK(obj.sequence == 148153491); };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}
