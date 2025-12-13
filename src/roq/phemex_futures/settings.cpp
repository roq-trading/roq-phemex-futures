/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/phemex_futures/settings.hpp"

#include "roq/logging.hpp"

using namespace std::literals;

namespace roq {
namespace phemex_futures {

// === IMPLEMENTATION ===

Settings::Settings(args::Parser const &args)
    : server::flags::Settings{args, ROQ_PACKAGE_NAME, ROQ_BUILD_NUMBER}, flags::Flags{flags::Flags::create()}, misc{flags::Misc::create()},
      rest{flags::REST::create()}, ws{flags::WS::create()} {
  log::info("settings={}"sv, *this);
}

}  // namespace phemex_futures
}  // namespace roq
