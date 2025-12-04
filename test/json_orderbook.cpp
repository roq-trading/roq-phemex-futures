/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_2_tester.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;

using value_type = json::Orderbook;

/*
// note! truncated
TEST_CASE("snapshot", "[json_orderbook]") {
  auto message = R"({)"
                 R"("book":{)"
                 R"("asks":[)"
                 R"([1094473000,1307083],)"
                 R"([1094528000,1806],)"
                 R"([1094759000,995],)"
                 R"([1094764000,238344])"
                 R"(],)"
                 R"("bids":[)"
                 R"([1094472000,190034],)"
                 R"([1094471000,804],)"
                 R"([1094253000,40019],)"
                 R"([1094251000,1])"
                 R"(])"
                 R"(},)"
                 R"("depth":30,)"
                 R"("sequence":20256544456,)"
                 R"("symbol":"BTCUSD",)"
                 R"("timestamp":1759032884308731357,)"
                 R"("type":"snapshot")"
                 R"(})";
  auto helper = [](value_type const &obj) { CHECK(obj.sequence == 20256544456); };
  Parser2Tester<value_type>::dispatch(helper, message, 8192, 3);
}

TEST_CASE("incremental", "[json_orderbook]") {
  auto message = R"({)"
                 R"("book":{)"
                 R"("asks":[],)"
                 R"("bids":[)"
                 R"([399734000000,0],)"
                 R"([398573000000,40000])"
                 R"(])"
                 R"(},)"
                 R"("depth":30,)"
                 R"("sequence":38514847027,)"
                 R"("symbol":"sETHUSDT",)"
                 R"("timestamp":1759033283091421157,)"
                 R"("type":"incremental")"
                 R"(})";
  auto helper = [](value_type const &obj) { CHECK(obj.sequence == 38514847027); };
  Parser2Tester<value_type>::dispatch(helper, message, 8192, 3);
}
*/
