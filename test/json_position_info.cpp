/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/phemex_futures/json/position_info.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;

TEST_CASE("usd_m_simple", "[json_position_info]") {
  auto message = R"({)"
                 R"("position_info":{)"
                 R"("accountID":88606880003,)"
                 R"("light":0,)"
                 R"("marginMode":"Cross",)"
                 R"("posSide":"Long",)"
                 R"("symbol":"ETHUSDT",)"
                 R"("userID":8860688)"
                 R"(},)"
                 R"("sequence":205057312)"
                 R"(})"sv;
  [[maybe_unused]] json::PositionInfo obj{message};
}
