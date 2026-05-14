/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <cstdint>

namespace roq {
namespace phemex_futures {

enum class RestState : uint8_t {
  UNDEFINED = 0,
  PRODUCTS,
  DONE,
};

}  // namespace phemex_futures
}  // namespace roq
