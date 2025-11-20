/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/phemex_futures/shared.hpp"

namespace roq {
namespace phemex_futures {

// === IMPLEMENTATION ===

Shared::Shared(server::Dispatcher &dispatcher, Settings const &settings)
    : dispatcher{dispatcher}, settings{settings}, api{API::create(settings)}, rate_limiter{settings.misc.request_limit, settings.misc.request_limit_interval},
      symbols{settings.ws.max_subscriptions_per_stream} {
}

}  // namespace phemex_futures
}  // namespace roq
