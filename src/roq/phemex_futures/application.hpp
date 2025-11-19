/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include "roq/service.hpp"

namespace roq {
namespace phemex_futures {

struct Application final : public roq::Service {
  using roq::Service::Service;

 protected:
  int main(args::Parser const &) override;
};

}  // namespace phemex_futures
}  // namespace roq
