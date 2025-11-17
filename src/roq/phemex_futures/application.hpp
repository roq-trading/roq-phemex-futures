/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include "roq/service.hpp"

namespace roq {
namespace phemex_futures {

class Application final : public roq::Service {
 public:
  using roq::Service::Service;

 protected:
  int main(args::Parser const &) override;
};

}  // namespace phemex_futures
}  // namespace roq
