/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/phemex_futures/json/parser.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;

// note! truncated
TEST_CASE("snapshot", "[json_kline]") {
  auto message = R"({)"
                 R"("kline":[)"
                 R"([1759040340,60,1093523000,1093522000,1093523000,1093522000,1093522000,94584,86494822],)"
                 R"([1759040280,60,1093522000,1093523000,1093523000,1093523000,1093523000,248,226789],)"
                 R"([1758920460,60,1092502000,1092503000,1092503000,1092503000,1092503000,1,915],)"
                 R"([1758920400,60,1093174000,1093111000,1093111000,1092502000,1092502000,106,96972])"
                 R"(],)"
                 R"("priceScale":4,)"
                 R"("sequence":20256660818,)"
                 R"("symbol":"BTCUSD",)"
                 R"("type":"snapshot",)"
                 R"("valueScale":8)"
                 R"(})"
                 R"(})";
  core::json::BufferStack buffers{8192, 2};
  // simple
  json::Kline obj{message, buffers};
  CHECK(obj.type == json::MessageType::SNAPSHOT);
  // parser
  struct Handler final : public json::Parser::Handler {
    void operator()(Trace<json::Pong> const &) override { FAIL(); }
    void operator()(Trace<json::Ack> const &) override { FAIL(); }
    void operator()(Trace<json::Book> const &) override { FAIL(); }
    void operator()(Trace<json::Trades> const &) override { FAIL(); }
    void operator()(Trace<json::Market24h> const &) override { FAIL(); }
    void operator()(Trace<json::Market24h2> const &) override { FAIL(); }
    void operator()(Trace<json::Kline> const &event) override {
      found = true;
      auto &[trace_info, kline] = event;
      CHECK(kline.type == json::MessageType::SNAPSHOT);
    }
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

TEST_CASE("incremental", "[json_kline]") {
  auto message = R"({)"
                 R"("kline":[)"
                 R"([1759040820,60,6459000,6468000,6483000,6468000,6483000,269388700000,17438876605])"
                 R"(],)"
                 R"("priceScale":8,)"
                 R"("qtyScale":8,)"
                 R"("sequence":18268599368,)"
                 R"("symbol":"sPUMPBTCUSDT",)"
                 R"("type":"incremental",)"
                 R"("valueScale":8)"
                 R"(})";
  core::json::BufferStack buffers{8192, 2};
  // simple
  json::Kline obj{message, buffers};
  CHECK(obj.type == json::MessageType::INCREMENTAL);
  // parser
  struct Handler final : public json::Parser::Handler {
    void operator()(Trace<json::Pong> const &) override { FAIL(); }
    void operator()(Trace<json::Ack> const &) override { FAIL(); }
    void operator()(Trace<json::Book> const &) override { FAIL(); }
    void operator()(Trace<json::Trades> const &) override { FAIL(); }
    void operator()(Trace<json::Market24h> const &) override { FAIL(); }
    void operator()(Trace<json::Market24h2> const &) override { FAIL(); }
    void operator()(Trace<json::Kline> const &event) override {
      found = true;
      auto &[trace_info, kline] = event;
      CHECK(kline.type == json::MessageType::INCREMENTAL);
    }
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
