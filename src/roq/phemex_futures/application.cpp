/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/phemex_futures/application.hpp"

#include "roq/phemex_futures/flags/settings.hpp"

#include "roq/phemex_futures/gateway/config.hpp"
#include "roq/phemex_futures/gateway/controller.hpp"

using namespace std::literals;

namespace roq {
namespace phemex_futures {

// === CONSTANTS ===

namespace {
uint8_t const API_COIN_M = 0x0;
uint8_t const API_USD_M = 0x1;
}  // namespace

// === HELPERS ===

namespace {
auto parse_api(auto &settings) {
  auto api = gateway::API::parse_api(settings);
  switch (api) {
    using enum gateway::API::Type;
    case COIN_M:
      return API_COIN_M;
    case USD_M:
      return API_USD_M;
  }
  log::fatal(R"(Unexpected: api="{}")"sv, settings.app.api);
}
}  // namespace

// === IMPLEMENTATION ===

int Application::main(args::Parser const &args) {
  flags::Settings settings{args};
  auto api = parse_api(settings);
  gateway::Config config{settings};
  log::info<1>("config={}"sv, config);
  auto context = server::create_io_context(settings);
  server::Trading2<gateway::Controller>(settings, config, *context, api).dispatch();
  return EXIT_SUCCESS;
}

}  // namespace phemex_futures
}  // namespace roq
