/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <fmt/format.h>

#include "roq/server/flags/settings.hpp"

#include "roq/phemex_futures/flags/flags.hpp"
#include "roq/phemex_futures/flags/misc.hpp"
#include "roq/phemex_futures/flags/rest.hpp"
#include "roq/phemex_futures/flags/ws.hpp"

namespace roq {
namespace phemex_futures {

struct Settings final : public server::flags::Settings, public flags::Flags {
  explicit Settings(args::Parser const &);

  flags::Misc misc;
  flags::REST rest;
  flags::WS ws;

 private:
  Settings(args::Parser const &, flags::Flags const &);
};

}  // namespace phemex_futures
}  // namespace roq

template <>
struct fmt::formatter<roq::phemex_futures::Settings> {
  constexpr auto parse(format_parse_context &context) { return std::begin(context); }
  auto format(roq::phemex_futures::Settings const &value, format_context &context) const {
    using namespace std::literals;
    return fmt::format_to(
        context.out(),
        R"({{)"
        R"(misc={}, )"
        R"(rest={}, )"
        R"(ws={}, )"
        R"(server={})"
        R"(}})"sv,
        value.misc,
        value.rest,
        value.ws,
        static_cast<roq::server::Settings const &>(value));
  }
};
