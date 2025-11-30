/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/phemex_futures/json/parser.hpp"

using namespace roq;
using namespace roq::phemex_futures;

using namespace std::literals;

// note! truncated
TEST_CASE("snapshot", "[json_book]") {
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
  core::json::BufferStack buffers{8192, 3};
  // simple
  json::Book obj{message, buffers};
  CHECK(obj.sequence == 20256544456);
  // parser
  struct Handler final : public json::Parser::Handler {
    void operator()(Trace<json::Pong> const &) override { FAIL(); }
    void operator()(Trace<json::Ack> const &) override { FAIL(); }
    void operator()(Trace<json::Book> const &event) override {
      found = true;
      auto &[trace_info, book] = event;
      CHECK(book.sequence == 20256544456);
    }
    void operator()(Trace<json::Trades> const &) override { FAIL(); }
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

TEST_CASE("incremental", "[json_book]") {
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
  core::json::BufferStack buffers{8192, 3};
  // simple
  json::Book obj{message, buffers};
  CHECK(obj.sequence == 38514847027);
  // parser
  struct Handler final : public json::Parser::Handler {
    void operator()(Trace<json::Pong> const &) override { FAIL(); }
    void operator()(Trace<json::Ack> const &) override { FAIL(); }
    void operator()(Trace<json::Book> const &event) override {
      found = true;
      auto &[trace_info, book] = event;
      CHECK(book.sequence == 38514847027);
    }
    void operator()(Trace<json::Trades> const &) override { FAIL(); }
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
