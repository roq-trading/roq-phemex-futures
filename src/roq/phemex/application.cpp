/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/phemex/application.hpp"

#include "roq/phemex/config.hpp"
#include "roq/phemex/gateway.hpp"
#include "roq/phemex/settings.hpp"

using namespace std::literals;

namespace roq {
namespace phemex {

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

}  // namespace phemex
}  // namespace roq
