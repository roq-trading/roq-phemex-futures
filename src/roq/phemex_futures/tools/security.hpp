/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include "roq/api.hpp"

namespace roq {
namespace phemex_futures {
namespace tools {

struct Security final {
  double quantity_factor = NaN;
  double price_factor = NaN;
};

}  // namespace tools
}  // namespace phemex_futures
}  // namespace roq
