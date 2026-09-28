/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <chrono>

#include <fmt/format.h>
#include <fmt/std.h>

#include "roq/web/rest/interceptor.hpp"

#include "roq/web/socket/interceptor.hpp"

#include "roq/server/settings.hpp"

namespace roq {
namespace phemex_futures {
namespace tools {

struct Throttle final : public web::rest::Interceptor, public web::socket::Interceptor {
  explicit Throttle(server::Settings const &);  // XXX HANS server::Settings ?

  struct Params {
    operator bool() const { return static_cast<bool>(retry_after); }

    void reset() {
      capacity.reset();
      remaining.reset();
      retry_after.reset();
    }

    std::optional<int32_t> capacity;
    std::optional<int32_t> remaining;
    std::optional<std::chrono::seconds> retry_after;
  };

 protected:
  // web::Interceptor
  operator std::chrono::nanoseconds() const override { return retry_after_; }

  // web::rest::Interceptor
  void operator()(Trace<web::rest::MessageBegin> const &) override;
  void operator()(Trace<web::rest::MessageHeader> const &) override;
  void operator()(Trace<web::rest::MessageEnd> const &) override;

  // web::socket::Interceptor

 private:
  bool const enabled_;

  Params global_;
  Params contract_;

  std::chrono::nanoseconds retry_after_ = {};
  std::chrono::nanoseconds retry_after_contract_ = {};
};

}  // namespace tools
}  // namespace phemex_futures
}  // namespace roq

template <>
struct fmt::formatter<roq::phemex_futures::tools::Throttle::Params> {
  constexpr auto parse(format_parse_context &context) { return std::begin(context); }
  auto format(roq::phemex_futures::tools::Throttle::Params const &value, format_context &context) const {
    using namespace std::literals;
    return fmt::format_to(
        context.out(),
        R"({{)"
        R"(capacity={}, )"
        R"(remaining={}, )"
        R"(retry_after={})"
        R"(}})"sv,
        value.capacity,
        value.remaining,
        value.retry_after);
  }
};
