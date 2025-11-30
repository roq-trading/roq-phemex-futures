/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/phemex_futures/json/parser.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;

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
  core::json::BufferStack buffers{8192, 1};
  // simple
  json::PositionInfo obj{message};
  CHECK(obj.sequence == 148153491);
  // parser
  struct Handler final : public json::Parser::Handler {
    void operator()(Trace<json::Pong> const &) override { FAIL(); }
    void operator()(Trace<json::Ack> const &) override { FAIL(); }
    void operator()(Trace<json::Book> const &) override { FAIL(); }
    void operator()(Trace<json::Trades> const &) override { FAIL(); }
    void operator()(Trace<json::Market24h> const &) override { FAIL(); }
    void operator()(Trace<json::Market24h2> const &) override { FAIL(); }
    void operator()(Trace<json::Kline> const &) override { FAIL(); }
    void operator()(Trace<json::IndexMarket24h> const &) override { FAIL(); }
    void operator()(Trace<json::AccountsOrdersPositions> const &) override { FAIL(); }
    void operator()(Trace<json::AccountsOrdersPositions2> const &) override { FAIL(); }
    void operator()(Trace<json::PositionInfo> const &event) override {
      found = true;
      auto &[trace_info, position_info] = event;
      CHECK(position_info.sequence == 148153491);
    }

    bool found = false;
  } handler;
  auto res = json::Parser::dispatch(handler, message, buffers, {}, false);
  CHECK(res == true);
  CHECK(handler.found == true);
}
