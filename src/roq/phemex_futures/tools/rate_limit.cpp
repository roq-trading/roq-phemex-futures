/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/phemex_futures/tools/rate_limit.hpp"

#include "roq/logging.hpp"

#include "roq/utils/compare.hpp"
#include "roq/utils/update.hpp"

#include "roq/utils/hash/fnv.hpp"

#include "roq/utils/charconv/from_chars.hpp"

using namespace std::literals;

namespace roq {
namespace phemex_futures {
namespace tools {

// === HELPERS ===

namespace {
// note! std::tolower is not constexpr gcc16 + clang23
constexpr auto lower(auto value) {
  return utils::detail::ascii_to_lower(value);
}

enum class Header {
  UNKNOWN,
  X_RATE_LIMIT_CAPACITY,
  X_RATE_LIMIT_REMAINING,
  X_RATE_LIMIT_RETRY_AFTER,
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
    case utils::hash::FNV::compute("x-ratelimit-capacity"sv):
      return Header::X_RATE_LIMIT_CAPACITY;
    case utils::hash::FNV::compute("x-ratelimit-remaining"sv):
      return Header::X_RATE_LIMIT_REMAINING;
    case utils::hash::FNV::compute("x-ratelimit-retry-after"sv):
      return Header::X_RATE_LIMIT_RETRY_AFTER;
    case utils::hash::FNV::compute("x-ratelimit-capacity-contract"sv):
      return Header::X_RATE_LIMIT_CAPACITY_CONTRACT;
    case utils::hash::FNV::compute("x-ratelimit-remaining-contract"sv):
      return Header::X_RATE_LIMIT_REMAINING_CONTRACT;
    case utils::hash::FNV::compute("x-ratelimit-retry-after-contract"sv):
      return Header::X_RATE_LIMIT_RETRY_AFTER_CONTRACT;
  }
  return Header::UNKNOWN;
}

static_assert(parse_header("X-RateLimit-Capacity"sv) == Header::X_RATE_LIMIT_CAPACITY);
static_assert(parse_header("X-RateLimit-Remaining"sv) == Header::X_RATE_LIMIT_REMAINING);
static_assert(parse_header("X-RateLimit-Retry-After"sv) == Header::X_RATE_LIMIT_RETRY_AFTER);
static_assert(parse_header("X-RateLimit-Capacity-CONTRACT"sv) == Header::X_RATE_LIMIT_CAPACITY_CONTRACT);
static_assert(parse_header("X-RateLimit-Remaining-CONTRACT"sv) == Header::X_RATE_LIMIT_REMAINING_CONTRACT);
static_assert(parse_header("X-RateLimit-Retry-After-CONTRACT"sv) == Header::X_RATE_LIMIT_RETRY_AFTER_CONTRACT);
}  // namespace

// === IMPLEMENTATION ===

// XXX FIXME TODO we could initialize with some flag about back-off policy

void RateLimit::operator()(Trace<web::rest::Client::Header> const &event) {
  auto &[trace_info, header] = event;
  auto update_value = [&](auto &result) {
    using value_type = std::remove_cvref_t<decltype(result)>;
    auto value = utils::charconv::from_chars<value_type>(header.value);
    return utils::update(result, value);
  };
  auto update_suspend_until = [&](auto &result, auto retry_after) {
    if (retry_after > 0) {
      auto tmp = trace_info.origin_create_time + std::chrono::seconds{global.retry_after};
      utils::update_max(result, tmp);
    } else {
      result = {};
    }
  };
  auto update_suspend_until_2 = [&]() { suspend_until = std::max(global.suspend_until, contract.suspend_until); };
  auto key = parse_header(header.name);
  switch (key) {
    using enum Header;
    [[likely]] case UNKNOWN:
      return;
    // global
    case X_RATE_LIMIT_CAPACITY:
      update_value(global.capacity);
      break;
    case X_RATE_LIMIT_REMAINING:
      if (update_value(global.remaining)) {
        if (global.remaining > 0) {
          global.suspend_until = {};
          update_suspend_until_2();
        }
      }
      break;
    case X_RATE_LIMIT_RETRY_AFTER:
      if (update_value(global.retry_after)) {
        update_suspend_until(global.suspend_until, global.retry_after);
        update_suspend_until_2();
      }
      break;
    // contract
    case X_RATE_LIMIT_CAPACITY_CONTRACT:
      update_value(contract.capacity);
      break;
    case X_RATE_LIMIT_REMAINING_CONTRACT:
      if (update_value(contract.remaining)) {
        if (contract.remaining > 0) {
          contract.suspend_until = {};
          update_suspend_until_2();
        }
      }
      break;
    case X_RATE_LIMIT_RETRY_AFTER_CONTRACT:
      if (update_value(contract.retry_after)) {
        update_suspend_until(contract.suspend_until, contract.retry_after);
        update_suspend_until_2();
      }
      break;
  }
}

}  // namespace tools
}  // namespace phemex_futures
}  // namespace roq
