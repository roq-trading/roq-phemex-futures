/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/phemex_futures/json/parser.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

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
  core::json::BufferStack buffers{8192, 1};
  // simple
  json::Market24h obj{message, buffers};
  CHECK(obj.timestamp == 1759029819812080202ns);
  // parser
  struct Handler final : public json::Parser::Handler {
    void operator()(Trace<json::Pong> const &) override { FAIL(); }
    void operator()(Trace<json::Ack> const &) override { FAIL(); }
    void operator()(Trace<json::Book> const &) override { FAIL(); }
    void operator()(Trace<json::Trades> const &) override { FAIL(); }
    void operator()(Trace<json::Market24h> const &event) override {
      found = true;
      auto &[trace_info, market_24h] = event;
      CHECK(market_24h.timestamp == 1759029819812080202ns);
    }
    void operator()(Trace<json::Market24h2> const &) override { FAIL(); }
    void operator()(Trace<json::Kline> const &) override { FAIL(); }
    void operator()(Trace<json::IndexMarket24h> const &) override { FAIL(); }
    void operator()(Trace<json::AccountsOrdersPositions> const &) override { FAIL(); }
    void operator()(Trace<json::AccountsOrdersPositions2> const &) override { FAIL(); }
    void operator()(Trace<json::PositionInfo> const &) override { FAIL(); }

    bool found = false;
  } handler;
  auto res = json::Parser::dispatch(handler, message, buffers, {}, false);
  CHECK(res == true);
  CHECK(handler.found == true);
}

TEST_CASE("usd_m_simple", "[json_market24h]") {
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
  core::json::BufferStack buffers{8192, 1};
  // simple
  json::Market24h2 obj{message, buffers};
  CHECK(obj.timestamp == 1763380521025768914ns);
  // parser
  struct Handler final : public json::Parser::Handler {
    void operator()(Trace<json::Pong> const &) override { FAIL(); }
    void operator()(Trace<json::Ack> const &) override { FAIL(); }
    void operator()(Trace<json::Book> const &) override { FAIL(); }
    void operator()(Trace<json::Trades> const &) override { FAIL(); }
    void operator()(Trace<json::Market24h> const &) override { FAIL(); }
    void operator()(Trace<json::Market24h2> const &event) override {
      found = true;
      auto &[trace_info, market_24h] = event;
      CHECK(market_24h.timestamp == 1763380521025768914ns);
    }
    void operator()(Trace<json::Kline> const &) override { FAIL(); }
    void operator()(Trace<json::IndexMarket24h> const &) override { FAIL(); }
    void operator()(Trace<json::AccountsOrdersPositions> const &) override { FAIL(); }
    void operator()(Trace<json::AccountsOrdersPositions2> const &) override { FAIL(); }
    void operator()(Trace<json::PositionInfo> const &) override { FAIL(); }

    bool found = false;
  } handler;
  auto res = json::Parser::dispatch(handler, message, buffers, {}, false);
  CHECK(res == true);
  CHECK(handler.found == true);
}
