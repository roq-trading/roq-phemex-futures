/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/phemex_futures/tools/crypto.hpp"

#include <fmt/format.h>

#include <cassert>

#include "roq/utils/codec/hex.hpp"

using namespace std::literals;

namespace roq {
namespace phemex_futures {
namespace tools {

// === IMPLEMENTATION ===

Crypto::Crypto(std::string_view const &key, std::string_view const &secret) : key_{key}, mac_{secret} {
}

std::string Crypto::create_ws_login(std::chrono::seconds now_utc, uint64_t request_id) {
  auto tmp = fmt::format("{}{}"sv, key_, now_utc.count());
  mac_.clear();
  mac_.update(tmp);
  auto digest = mac_.final(digest_);
  std::string signature;
  utils::codec::Hex::encode(signature, digest);
  auto result = fmt::format(
      R"({{)"
      R"("id":{},)"
      R"("method":"user.auth",)"
      R"("params":[)"
      R"("API",)"
      R"("{}",)"
      R"("{}",)"
      R"({})"
      R"(])"
      R"(}})"sv,
      request_id,
      key_,
      signature,
      now_utc.count());
  return result;
}

std::string_view Crypto::create_headers(
    std::string_view const &path,
    std::string_view const &query,
    std::string_view const &body,
    std::chrono::seconds now_utc,
    std::string_view const &request_id) {
  assert(!std::empty(path));
  auto tmp = fmt::format("{}{}{}{}"sv, path, query, now_utc.count(), body);
  mac_.clear();
  mac_.update(tmp);
  auto digest = mac_.final(digest_);
  std::string signature;
  utils::codec::Hex::encode(signature, digest);
  encode_buffer_.clear();
  fmt::format_to(
      std::back_inserter(encode_buffer_),
      "x-phemex-access-token: {}\r\n"
      "x-phemex-request-expiry: {}\r\n"
      "x-phemex-request-signature: {}\r\n"sv,
      key_,
      now_utc.count(),
      signature);
  if (!std::empty(request_id)) {
    fmt::format_to(std::back_inserter(encode_buffer_), "x-phemex-request-tracing: {}\r\n"sv, request_id);
  }
  return encode_buffer_;
}

}  // namespace tools
}  // namespace phemex_futures
}  // namespace roq
