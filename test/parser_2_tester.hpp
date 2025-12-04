/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/phemex_futures/json/parser_2.hpp"

namespace roq {
namespace phemex_futures {

template <typename T>
struct Parser2Tester final : public json::Parser2::Handler {
  using value_type = std::remove_cvref_t<T>;
  using callback_type = std::function<void(value_type const &)>;

  static void dispatch(callback_type const &callback, std::string_view const &message, size_t buffer_size, size_t max_depth) {
    core::json::BufferStack buffers{buffer_size, max_depth};
    // simple
    // XXX FIXME TODO catch2 block ???
    T obj{message, buffers};
    callback(obj);
    // parser
    // XXX FIXME TODO catch2 block ???
    Parser2Tester handler{callback};
    auto res = json::Parser2::dispatch(handler, message, buffers, {}, false);
    CHECK(res == true);
    CHECK(handler.found_ == true);
  }

 protected:
  explicit Parser2Tester(callback_type const &callback) : callback_{callback} {}

  void operator()(Trace<json::Pong> const &event) override { dispatch(event); }
  void operator()(Trace<json::Ack> const &event) override { dispatch(event); }
  void operator()(Trace<json::Orderbook> const &event) override { dispatch(event); }
  void operator()(Trace<json::Trades2> const &event) override { dispatch(event); }
  void operator()(Trace<json::Market24h2> const &event) override { dispatch(event); }
  void operator()(Trace<json::Kline2> const &event) override { dispatch(event); }
  void operator()(Trace<json::IndexMarket24h> const &event) override { dispatch(event); }
  void operator()(Trace<json::AccountsOrdersPositions2> const &event) override { dispatch(event); }
  void operator()(Trace<json::PositionInfo> const &event) override { dispatch(event); }

  template <typename U>
  void dispatch(Trace<U> const &event) {
    if constexpr (std::is_invocable_v<callback_type, U>) {
      found_ = true;
      callback_(event);
    } else {
      FAIL();
    }
  }

 private:
  callback_type const callback_;
  bool found_ = false;
};

}  // namespace phemex_futures
}  // namespace roq
