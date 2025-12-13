/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_2_tester.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;

using value_type = json::Trades2;

// note! truncated
TEST_CASE("snapshot", "[json_trades_2]") {
  auto message = R"({)"
                 R"("dts":1764824672044962236,)"
                 R"("mts":1764824662377238082,)"
                 R"("sequence":7438923573,)"
                 R"("symbol":"BTCUSDC",)"
                 R"("trades_p":[)"
                 R"([1764824662350987242,"Buy","93284","0.007"],)"
                 R"([1764821929539054805,"Sell","93418.2","0.006"])"
                 R"(],)"
                 R"("type":"snapshot")"
                 R"(})";
  auto helper = [](value_type const &obj) { CHECK(obj.dts == 1764824672044962236ns); };
  Parser2Tester<value_type>::dispatch(helper, message, 8192, 2);
}

TEST_CASE("incremental", "[json_trades_2]") {
  auto message = R"({)"
                 R"("dts":1764824672795878735,)"
                 R"("mts":1764824672795057041,)"
                 R"("sequence":54193715981,)"
                 R"("symbol":"APTUSDT",)"
                 R"("trades_p":[)"
                 R"([1764824672778886833,"Buy","2.008","247.7"],)"
                 R"([1764824672778886833,"Buy","2.008","266.1"])"
                 R"(],)"
                 R"("type":"incremental")"
                 R"(})";
  auto helper = [](value_type const &obj) { CHECK(obj.dts == 1764824672795878735ns); };
  Parser2Tester<value_type>::dispatch(helper, message, 8192, 2);
}
