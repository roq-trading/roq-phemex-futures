/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/phemex_futures/application.hpp"

#include "roq/phemex_futures/config.hpp"
#include "roq/phemex_futures/gateway.hpp"
#include "roq/phemex_futures/settings.hpp"

using namespace std::literals;

namespace roq {
namespace phemex_futures {

// === CONSTANTS ===

namespace {
uint8_t const API_2 = {};
}

// === IMPLEMENTATION ===

int Application::main(args::Parser const &args) {
  Settings settings{args};
  Config config{settings};
  log::info<1>("config={}"sv, config);
  auto context = server::create_io_context(settings);
  server::Trading<Gateway>(settings, config, *context, API_2).dispatch();
  return EXIT_SUCCESS;
}

}  // namespace phemex_futures
}  // namespace roq
