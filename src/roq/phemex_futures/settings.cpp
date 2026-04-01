/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/phemex_futures/settings.hpp"

#include "roq/logging.hpp"

using namespace std::literals;

namespace roq {
namespace phemex_futures {

// === IMPLEMENTATION ===

Settings::Settings(args::Parser const &args) : Settings{args, flags::Flags::create()} {
}

Settings::Settings(args::Parser const &args, flags::Flags const &flags)
    : server::flags::Settings{args, ROQ_PACKAGE_NAME, ROQ_BUILD_NUMBER, flags.api}, flags::Flags{flags}, misc{flags::Misc::create()},
      rest{flags::REST::create()}, ws{flags::WS::create()} {
  log::info("settings={}"sv, *this);
}

}  // namespace phemex_futures
}  // namespace roq
