/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include <string>
#include <string_view>

#include "roq/phemex/config.hpp"

#include "roq/phemex/tools/crypto.hpp"

namespace roq {
namespace phemex {

class Account final {
 public:
  Account(Config const &, std::string_view const &name);

  Account(Account const &) = delete;

  std::string create_ws_login(uint64_t request_id);

  std::string_view create_headers(std::string_view const &path, std::string_view const &query, std::string_view const &body);
  std::string_view create_headers(
      std::string_view const &path, std::string_view const &query, std::string_view const &body, std::string_view const &request_id);

  std::string const name;

 private:
  tools::Crypto crypto_;
};

}  // namespace phemex
}  // namespace roq
