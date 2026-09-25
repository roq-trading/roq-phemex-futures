/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <chrono>

#include "roq/web/rest/client.hpp"
#include "roq/web/rest/response.hpp"

namespace roq {
namespace phemex_futures {
namespace tools {

struct RateLimit final {
  RateLimit() = default;

  RateLimit(RateLimit &&) = default;
  RateLimit(RateLimit const &) = delete;

  operator std::chrono::nanoseconds() const { return suspend_until_; }

  void operator()(Trace<web::rest::Client::Header> const &);
  void operator()(Trace<web::rest::Response> const &);

 private:
  struct item_t {
    int32_t capacity = {};
    int32_t remaining = {};
    int32_t retry_after = {};
    std::chrono::nanoseconds suspend_until = {};
  };

  item_t global_;
  item_t contract_;

  std::chrono::nanoseconds suspend_until_ = {};
};

}  // namespace tools
}  // namespace phemex_futures
}  // namespace roq
