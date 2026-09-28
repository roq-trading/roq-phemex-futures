/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/phemex_futures/tools/throttle.hpp"

#include "roq/utils/compare.hpp"  // ascii_to_lower
#include "roq/utils/traits.hpp"
#include "roq/utils/update.hpp"

#include "roq/utils/hash/fnv.hpp"

#include "roq/utils/charconv/from_chars.hpp"

using namespace std::literals;

namespace roq {
namespace phemex_futures {
namespace tools {

// === CONSTANTS ===

namespace {
auto const DEFAULT_BACKOFF = 60s;
}

// === HELPERS ===

namespace {
// note! std::tolower is not constexpr gcc16 + clang23
constexpr auto lower(auto value) {
  return utils::detail::ascii_to_lower(value);
}

enum class Header {
  UNKNOWN,
  // global
  X_RATE_LIMIT_CAPACITY,
  X_RATE_LIMIT_REMAINING,
  X_RATE_LIMIT_RETRY_AFTER,
  // group: contract
  X_RATE_LIMIT_CAPACITY_CONTRACT,
  X_RATE_LIMIT_REMAINING_CONTRACT,
  X_RATE_LIMIT_RETRY_AFTER_CONTRACT,
};

constexpr auto parse_header(std::string_view const &text) {
  std::string value;
  value.reserve(std::size(text));
  std::transform(std::begin(text), std::end(text), std::back_inserter(value), [](auto c) { return lower(c); });
  auto key = utils::hash::FNV::compute(value);
  switch (key) {
    // global
    case utils::hash::FNV::compute("x-ratelimit-capacity"sv):
      return Header::X_RATE_LIMIT_CAPACITY;
    case utils::hash::FNV::compute("x-ratelimit-remaining"sv):
      return Header::X_RATE_LIMIT_REMAINING;
    case utils::hash::FNV::compute("x-ratelimit-retry-after"sv):
      return Header::X_RATE_LIMIT_RETRY_AFTER;
    // group: contract
    case utils::hash::FNV::compute("x-ratelimit-capacity-contract"sv):
      return Header::X_RATE_LIMIT_CAPACITY_CONTRACT;
    case utils::hash::FNV::compute("x-ratelimit-remaining-contract"sv):
      return Header::X_RATE_LIMIT_REMAINING_CONTRACT;
    case utils::hash::FNV::compute("x-ratelimit-retry-after-contract"sv):
      return Header::X_RATE_LIMIT_RETRY_AFTER_CONTRACT;
  }
  return Header::UNKNOWN;
}

// global
static_assert(parse_header("X-RateLimit-Capacity"sv) == Header::X_RATE_LIMIT_CAPACITY);
static_assert(parse_header("X-RateLimit-Remaining"sv) == Header::X_RATE_LIMIT_REMAINING);
static_assert(parse_header("X-RateLimit-Retry-After"sv) == Header::X_RATE_LIMIT_RETRY_AFTER);
// group: contract
static_assert(parse_header("X-RateLimit-Capacity-CONTRACT"sv) == Header::X_RATE_LIMIT_CAPACITY_CONTRACT);
static_assert(parse_header("X-RateLimit-Remaining-CONTRACT"sv) == Header::X_RATE_LIMIT_REMAINING_CONTRACT);
static_assert(parse_header("X-RateLimit-Retry-After-CONTRACT"sv) == Header::X_RATE_LIMIT_RETRY_AFTER_CONTRACT);
}  // namespace

// === IMPLEMENTATION ===

Throttle::Throttle(server::Settings const &settings) : enabled_{settings.experimental.enable_rate_limit} {
}

// web::rest::Interceptor

void Throttle::operator()(Trace<web::rest::MessageBegin> const &) {
  global_.reset();
  contract_.reset();
}

void Throttle::operator()(Trace<web::rest::MessageHeader> const &event) {
  auto &[trace_info, header] = event;
  auto update_value = [&](auto &result) {
    using value_type = std::remove_cvref_t<decltype(result)>::value_type;
    if constexpr (utils::is_duration_v<value_type>) {
      auto value = utils::charconv::from_chars<int64_t>(header.value);
      result = value_type{value};
    } else {
      auto value = utils::charconv::from_chars<value_type>(header.value);
      result = value;
    }
  };
  auto key = parse_header(header.name);
  switch (key) {
    using enum Header;
    [[likely]] case UNKNOWN:
      return;
    // global
    case X_RATE_LIMIT_CAPACITY:
      update_value(global_.capacity);
      break;
    case X_RATE_LIMIT_REMAINING:
      update_value(global_.remaining);
      break;
    case X_RATE_LIMIT_RETRY_AFTER:
      update_value(global_.retry_after);
      break;
    // group: contract
    case X_RATE_LIMIT_CAPACITY_CONTRACT:
      update_value(contract_.capacity);
      break;
    case X_RATE_LIMIT_REMAINING_CONTRACT:
      update_value(contract_.remaining);
      break;
    case X_RATE_LIMIT_RETRY_AFTER_CONTRACT:
      update_value(contract_.retry_after);
      break;
  }
}

void Throttle::operator()(Trace<web::rest::MessageEnd> const &event) {
  auto &[trace_info, message_end] = event;
  if (enabled_) {
    if (global_) {
      auto retry_after = std::chrono::duration_cast<std::chrono::nanoseconds>(global_.retry_after.value());
      retry_after_ = std::max(retry_after_, retry_after);
    }
    if (contract_) {
      auto retry_after = std::chrono::duration_cast<std::chrono::nanoseconds>(contract_.retry_after.value());
      retry_after_contract_ = std::max(retry_after_contract_, retry_after);
    }
    switch (message_end.status) {
      using enum web::http::Status;
      // status code for hard backoff ???
      [[unlikely]] case TOO_MANY_REQUESTS: {  // 429
        // global => soft backoff on all
        if (retry_after_.count() == 0) {
          auto now = clock::get_system();
          retry_after_ = now + DEFAULT_BACKOFF;
        }
        break;
      }
      default:
        break;
    }
  }
}

// web::socket::Interceptor

}  // namespace tools
}  // namespace phemex_futures
}  // namespace roq
