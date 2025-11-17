/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/phemex/account.hpp"

#include "roq/utils/safe_cast.hpp"

#include "roq/clock.hpp"

using namespace std::chrono_literals;

namespace roq {
namespace phemex {

// === CONSTANTS ===

namespace {
auto const TIMEOUT = 60s;
}

// === HELPERS ===

namespace {
template <typename R>
auto create_crypto(auto &config, auto &name) {
  using result_type = std::remove_cvref_t<R>;
  return result_type{config.get_api_key(name), config.get_secret(name)};
}
}  // namespace

// === IMPLEMENTATION ===

Account::Account(Config const &config, std::string_view const &name) : name{name}, crypto_{create_crypto<decltype(crypto_)>(config, name)} {
}

std::string Account::create_ws_login(uint64_t request_id) {
  auto now_utc = clock::get_realtime<std::chrono::seconds>();
  return crypto_.create_ws_login(now_utc, request_id);
}

std::string_view Account::create_headers(std::string_view const &path, std::string_view const &query, std::string_view const &body) {
  return create_headers(path, query, body, {});
}

std::string_view Account::create_headers(
    std::string_view const &path, std::string_view const &query, std::string_view const &body, std::string_view const &request_id) {
  auto query_2 = std::empty(query) ? query : query.substr(1);
  auto now_utc = clock::get_realtime<std::chrono::seconds>() + TIMEOUT;
  return crypto_.create_headers(path, query_2, body, now_utc, request_id);
}

}  // namespace phemex
}  // namespace roq
