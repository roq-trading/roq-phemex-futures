/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <chrono>

#include "roq/web/rest/client.hpp"

namespace roq {
namespace phemex_futures {
namespace tools {

struct RateLimit final {
  RateLimit() = default;

  RateLimit(RateLimit &&) = default;
  RateLimit(RateLimit const &) = delete;

  void operator()(Trace<web::rest::Client::Header> const &);

  struct item_t {
    int32_t capacity = {};
    int32_t remaining = {};
    int32_t retry_after = {};
    std::chrono::nanoseconds suspend_until = {};
  };

  item_t global;
  item_t contract;

  std::chrono::nanoseconds suspend_until = {};
};

}  // namespace tools
}  // namespace phemex_futures
}  // namespace roq
