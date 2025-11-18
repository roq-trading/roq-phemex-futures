/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include <array>
#include <chrono>
#include <string>
#include <string_view>

#include "roq/utils/mac/hmac.hpp"

namespace roq {
namespace phemex_futures {
namespace tools {

struct Crypto final {
  Crypto(std::string_view const &key, std::string_view const &secret);

  Crypto(Crypto &&) = delete;
  Crypto(Crypto const &) = delete;

  std::string create_ws_login(std::chrono::seconds now_utc, uint64_t request_id);

  std::string_view create_headers(
      std::string_view const &path,
      std::string_view const &query,
      std::string_view const &body,
      std::chrono::seconds now_utc,
      std::string_view const &request_id);

 private:
  using MAC = utils::mac::HMAC<utils::hash::SHA256>;
  using Digest = std::array<std::byte, MAC::DIGEST_LENGTH>;

  std::string const key_;
  MAC mac_;
  Digest digest_;

  std::string encode_buffer_;
};

}  // namespace tools
}  // namespace phemex_futures
}  // namespace roq
