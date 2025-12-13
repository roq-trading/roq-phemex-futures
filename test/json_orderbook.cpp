/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_2_tester.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;

using value_type = json::Orderbook;

// note! truncated
TEST_CASE("snapshot", "[json_orderbook]") {
  auto message = R"({)"
                 R"("depth":30,)"
                 R"("dts":1764824672041508855,)"
                 R"("mts":1764824671534602922,)"
                 R"("orderbook_p":{)"
                 R"("asks":[)"
                 R"(["93277.9","0.0084"],)"
                 R"(["93286.5","0.049"],)"
                 R"(["93596.9","1.842"],)"
                 R"(["93644.4","1.969"])"
                 R"(],)"
                 R"("bids":[)"
                 R"(["93277.8","0.0692"],)"
                 R"(["93271.9","2.722"],)"
                 R"(["92973.8","2.567"],)"
                 R"(["92966.5","1.767"])"
                 R"(])"
                 R"(},)"
                 R"("sequence":7439467592,)"
                 R"("symbol":"BTCUSDC",)"
                 R"("timestamp":1764824671508301786,)"
                 R"("type":"snapshot")"
                 R"(})";
  auto helper = [](value_type const &obj) { CHECK(obj.depth == 30); };
  Parser2Tester<value_type>::dispatch(helper, message, 8192, 3);
}

TEST_CASE("incremental", "[json_orderbook]") {
  auto message = R"({)"
                 R"("depth":30,)"
                 R"("dts":1764824672060970046,)"
                 R"("mts":1764824672060514907,)"
                 R"("orderbook_p":{)"
                 R"("asks":[],)"
                 R"("bids":[)"
                 R"(["93237.3","0.3755"])"
                 R"(])"
                 R"(},)"
                 R"("sequence":50908054005,)"
                 R"("symbol":"BTCUSDT",)"
                 R"("timestamp":1764824672042451839,)"
                 R"("type":"incremental")"
                 R"(})";
  auto helper = [](value_type const &obj) { CHECK(obj.depth == 30); };
  Parser2Tester<value_type>::dispatch(helper, message, 8192, 3);
}
