/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <chrono>
#include <string>
#include <utility>
#include <vector>

#include "roq/api.hpp"
#include "roq/server.hpp"

#include "roq/core/symbols.hpp"

#include "roq/core/limit/rate_limiter.hpp"

#include "roq/phemex_futures/gateway/api.hpp"
#include "roq/phemex_futures/gateway/settings.hpp"

#include "roq/phemex_futures/tools/currency.hpp"
#include "roq/phemex_futures/tools/security.hpp"

namespace roq {
namespace phemex_futures {
namespace gateway {

struct Shared final {
  Shared(server::Dispatcher &, Settings const &);

  Shared(Shared const &) = delete;

  auto discard_symbol(std::string_view const &name) const { return dispatcher.discard_symbol(name); }

  template <typename... Args>
  auto operator()(Args &&...args) {
    return dispatcher(std::forward<Args>(args)...);
  }

 public:
  std::vector<MBPUpdate> bids, asks, final_bids, final_asks;
  std::vector<Trade> trades;
  std::vector<Fill> fills;

 public:
  server::Dispatcher &dispatcher;

 public:
  Settings const &settings;
  API const api;
  core::limit::RateLimiter rate_limiter;
  core::Symbols symbols;
  utils::unordered_set<std::string> all_symbols;

  // currency

  utils::unordered_map<std::string, tools::Currency> currency;

  template <typename Callback>
  bool find_currency(std::string_view const &name, Callback callback) {
    auto iter = currency.find(name);
    if (iter == std::end(currency)) {
      return false;
    }
    callback((*iter).second);
    return true;
  }

  // security

  utils::unordered_map<std::string, tools::Security> security;

  template <typename Callback>
  bool find_security(std::string_view const &symbol, Callback callback) {
    auto iter = security.find(symbol);
    if (iter == std::end(security)) {
      return false;
    }
    callback((*iter).second);
    return true;
  }
};

}  // namespace gateway
}  // namespace phemex_futures
}  // namespace roq
