/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include "roq/compat.hpp"

#include <memory>
#include <string>
#include <vector>

#include "roq/server.hpp"

#include "roq/io/context.hpp"

#include "roq/server/stream.hpp"

#include "roq/phemex_futures/gateway/account.hpp"
#include "roq/phemex_futures/gateway/config.hpp"
#include "roq/phemex_futures/gateway/settings.hpp"
#include "roq/phemex_futures/gateway/shared.hpp"

#include "roq/phemex_futures/gateway/drop_copy_coin_m.hpp"
#include "roq/phemex_futures/gateway/market_data_coin_m.hpp"
#include "roq/phemex_futures/gateway/order_entry_coin_m.hpp"
#include "roq/phemex_futures/gateway/rest_coin_m.hpp"

#include "roq/phemex_futures/gateway/drop_copy_usd_m.hpp"
#include "roq/phemex_futures/gateway/market_data_usd_m.hpp"
#include "roq/phemex_futures/gateway/order_entry_usd_m.hpp"
#include "roq/phemex_futures/gateway/rest_usd_m.hpp"

namespace roq {
namespace phemex_futures {
namespace gateway {

struct Controller final : public server::Handler,
                          public RestUsdM::Handler,
                          public RestCoinM::Handler,
                          public MarketDataUsdM::Handler,
                          public MarketDataCoinM::Handler,
                          public OrderEntryUsdM::Handler,
                          public OrderEntryCoinM::Handler,
                          public DropCopyUsdM::Handler,
                          public DropCopyCoinM::Handler {
  // public OrderEntry::Handler, public DropCopy::Handler, public MarketData::Handler {
  ROQ_PUBLIC static std::unique_ptr<server::Handler> create(server::Dispatcher &, Settings const &, Config const &, io::Context &);

  ROQ_PUBLIC static uint8_t parse_api(Settings const &);

  Controller(server::Dispatcher &, Settings const &, Config const &, io::Context &);

  Controller(Controller const &) = delete;

 protected:
  // server::Handler

  void operator()(Trace<Start> const &) override;
  void operator()(Trace<Stop> const &) override;
  void operator()(Trace<Timer> const &) override;

  void operator()(Event<Connected> const &) override;
  void operator()(Event<Disconnected> const &) override;

  void operator()(Event<Subscribe> const &) override;
  void operator()(Event<Control> const &) override;

  uint16_t operator()(Event<CreateOrder> const &, server::oms::Order const &, server::oms::RefData const &, std::string_view const &request_id) override;
  uint16_t operator()(
      Event<ModifyOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id) override;
  uint16_t operator()(
      Event<CancelOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id) override;

  uint16_t operator()(Event<CancelAllOrders> const &, std::string_view const &request_id) override;

  uint16_t operator()(Event<MassQuote> const &) override;

  uint16_t operator()(Event<CancelQuotes> const &) override;

  void operator()(metrics::Writer &) const override;

  // RestUsdM::Handler

  void operator()(RestUsdM::SymbolsUpdate &) override;

  // RestCoinM::Handler

  void operator()(RestCoinM::SymbolsUpdate &) override;

  // helpers

  void ensure_symbol_slices(size_t size);

  void subscribe_helper(auto &symbols);

  template <typename... Args>
  void dispatch(Args &&...);

  template <typename... Args>
  static void dispatch_helper(auto &self, Args &&...);

  server::OrderActionStream &get_order_entry(std::string_view const &account);

  struct OrderEntryRR final {
    OrderEntryRR(std::vector<std::unique_ptr<server::OrderActionStream>> &&);

    template <typename... Args>
    void operator()(Args &&...);

    template <typename... Args>
    void operator()(Args &&...) const;

    server::OrderActionStream &get_next();

   private:
    std::vector<std::unique_ptr<server::OrderActionStream>> order_entry_;
    size_t index_ = {};
  };

 private:
  server::Dispatcher &dispatcher_;
  // config
  std::string const master_account_;
  // accounts
  utils::unordered_map<std::string, std::unique_ptr<Account>> const accounts_;
  // io
  io::Context &context_;
  // shared
  Shared shared_;
  // seed
  uint16_t stream_id_ = {};
  // streams
  std::unique_ptr<server::Stream> rest_;
  utils::unordered_map<std::string, OrderEntryRR> order_entry_;
  utils::unordered_map<std::string, std::unique_ptr<server::Stream>> drop_copy_;
  std::vector<std::unique_ptr<server::MarketDataStream>> market_data_;
};

}  // namespace gateway
}  // namespace phemex_futures
}  // namespace roq
