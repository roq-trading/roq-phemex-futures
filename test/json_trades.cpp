/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/phemex_futures/json/parser.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;

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
  core::json::BufferStack buffers{8192, 2};
  // simple
  json::Trades obj{message, buffers};
  CHECK(obj.sequence == 20254182892);
  // parser
  struct Handler final : public json::Parser::Handler {
    void operator()(Trace<json::Pong> const &) override { FAIL(); }
    void operator()(Trace<json::Ack> const &) override { FAIL(); }
    void operator()(Trace<json::Book> const &) override { FAIL(); }
    void operator()(Trace<json::Trades> const &event) override {
      found = true;
      auto &[trace_info, trades] = event;
      CHECK(trades.sequence == 20254182892);
    }
    void operator()(Trace<json::Market24h> const &) override { FAIL(); }
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
