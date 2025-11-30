/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/phemex_futures/json/parser.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

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
  core::json::BufferStack buffers{8192, 1};
  // simple
  json::IndexMarket24h obj{message, buffers};
  CHECK(obj.timestamp == 1763353202275046666ns);
  // parser
  struct Handler final : public json::Parser::Handler {
    void operator()(Trace<json::Pong> const &) override { FAIL(); }
    void operator()(Trace<json::Ack> const &) override { FAIL(); }
    void operator()(Trace<json::Book> const &) override { FAIL(); }
    void operator()(Trace<json::Trades> const &) override { FAIL(); }
    void operator()(Trace<json::Market24h> const &) override { FAIL(); }
    void operator()(Trace<json::Market24h2> const &) override { FAIL(); }
    void operator()(Trace<json::Kline> const &) override { FAIL(); }
    void operator()(Trace<json::IndexMarket24h> const &event) override {
      found = true;
      auto &[trace_info, index_market_24h] = event;
      CHECK(index_market_24h.timestamp == 1763353202275046666ns);
    }
    void operator()(Trace<json::AccountsOrdersPositions> const &) override { FAIL(); }
    void operator()(Trace<json::AccountsOrdersPositions2> const &) override { FAIL(); }
    void operator()(Trace<json::PositionInfo> const &) override { FAIL(); }

    bool found = false;
  } handler;
  auto res = json::Parser::dispatch(handler, message, buffers, {}, false);
  CHECK(res == true);
  CHECK(handler.found == true);
}
