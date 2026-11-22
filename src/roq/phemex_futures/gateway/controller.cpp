/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/phemex_futures/gateway/controller.hpp"

#include "roq/logging.hpp"

#include "roq/server/oms/exceptions.hpp"

#include "roq/phemex_futures/gateway/api.hpp"

#include "roq/phemex_futures/gateway/drop_copy_coin_m.hpp"
#include "roq/phemex_futures/gateway/market_data_coin_m.hpp"
#include "roq/phemex_futures/gateway/order_entry_coin_m.hpp"
#include "roq/phemex_futures/gateway/rest_coin_m.hpp"

#include "roq/phemex_futures/gateway/drop_copy_usd_m.hpp"
#include "roq/phemex_futures/gateway/market_data_usd_m.hpp"
#include "roq/phemex_futures/gateway/order_entry_usd_m.hpp"
#include "roq/phemex_futures/gateway/rest_usd_m.hpp"

#include "roq/phemex_futures/protocol/json/utils.hpp"

using namespace std::literals;

namespace roq {
namespace phemex_futures {
namespace gateway {

// === CONSTANTS ===

namespace {
uint8_t const API_COIN_M = 0x1;
uint8_t const API_USD_M = 0x2;
}  // namespace

// === HELPERS ===

namespace {
template <typename R>
auto create_accounts(auto &config) {
  using result_type = std::remove_cvref_t<R>;
  result_type result;
  for (auto &[_, account] : config.accounts) {
    auto obj = std::make_unique<Account>(config, account.name);
    result.try_emplace(account.name, std::move(obj));
  }
  return result;
}

template <typename R>
R create_requests(auto &config, auto &shared) {
  using result_type = std::remove_cvref_t<R>;
  result_type result;
  for (auto &[_, account] : config.accounts) {
    auto obj = std::make_unique<Request>(shared);
    result.try_emplace(account.name, std::move(obj));
  }
  return result;
}

Request &get_request(auto &requests, auto &account) {
  auto iter = requests.find(account);
  if (iter == std::end(requests)) {
    log::fatal("Unexpected"sv);
  }
  return *(*iter).second;
}

template <typename R>
auto create_rest(auto &gateway, auto &context, auto &stream_id, auto &shared, auto &accounts, auto &master_account) {
  using result_type = std::remove_cvref_t<R>;
  result_type result;
  auto &account = [&]() -> Account & {
    auto iter = accounts.find(master_account);
    if (iter == std::end(accounts)) [[unlikely]] {
      throw RuntimeError{R"(Unknown account="{}")"sv, master_account};
    }
    return *(*iter).second;
  }();
  switch (shared.api.type) {
    using enum API::Type;
    case COIN_M:
      result = std::make_unique<RestCoinM>(gateway, context, ++stream_id, shared);
      break;
    case USD_M:
      result = std::make_unique<RestUsdM>(gateway, context, ++stream_id, shared, account);
      break;
  }
  return result;
}

template <typename R>
auto create_order_entry(auto &gateway, auto &context, auto &stream_id, auto &accounts, auto &shared, auto &requests) {
  using result_type = std::remove_cvref_t<R>;
  result_type result;
  if (!shared.settings.misc.number_of_order_entry_connections) {
    log::fatal("Unexpected: --number_of_order_entry_connections={}"sv, shared.settings.misc.number_of_order_entry_connections);
  }
  for (auto &[_, item] : accounts) {
    auto &account = *item;
    auto &request = get_request(requests, account.name);
    std::vector<std::unique_ptr<server::OrderActionStream>> order_entry;
    for (size_t i = 0; i < shared.settings.misc.number_of_order_entry_connections; ++i) {
      auto master = i == 0;
      switch (shared.api.type) {
        using enum API::Type;
        case COIN_M: {
          auto obj = std::make_unique<OrderEntryCoinM>(gateway, context, ++stream_id, account, shared, request, master);
          order_entry.emplace_back(std::move(obj));
          break;
        }
        case USD_M: {
          auto obj = std::make_unique<OrderEntryUsdM>(gateway, context, ++stream_id, account, shared, request, master);
          order_entry.emplace_back(std::move(obj));
          break;
        }
      }
    }
    result.try_emplace(account.name, std::move(order_entry));
  }
  return result;
}

template <typename R>
auto create_drop_copy(auto &gateway, auto &context, auto &stream_id, auto &accounts, auto &shared, auto &requests) {
  using result_type = std::remove_cvref_t<R>;
  result_type result;
  switch (shared.api.type) {
    using enum API::Type;
    case COIN_M:
      for (auto &[_, item] : accounts) {
        auto &account = *item;
        auto &request = get_request(requests, account.name);
        auto obj = std::make_unique<DropCopyCoinM>(gateway, context, ++stream_id, account, shared, request);
        result.try_emplace(account.name, std::move(obj));
      }
      break;
    case USD_M:
      for (auto &[_, item] : accounts) {
        auto &account = *item;
        auto &request = get_request(requests, account.name);
        auto obj = std::make_unique<DropCopyUsdM>(gateway, context, ++stream_id, account, shared, request);
        result.try_emplace(account.name, std::move(obj));
      }
      break;
  }
  return result;
}
}  // namespace

// === IMPLEMENTATION ===

std::unique_ptr<server::Handler> Controller::create(server::Dispatcher &dispatcher, Settings const &settings, Config const &config, io::Context &context) {
  return std::make_unique<Controller>(dispatcher, settings, config, context);
}

uint8_t Controller::parse_api(Settings const &settings) {
  auto api = API::parse_api(settings);
  switch (api) {
    using enum API::Type;
    case COIN_M:
      return API_COIN_M;
    case USD_M:
      return API_USD_M;
  }
  log::fatal(R"(Unexpected: api="{}")"sv, settings.app.api);
}

Controller::Controller(server::Dispatcher &dispatcher, Settings const &settings, Config const &config, io::Context &context)
    : dispatcher_{dispatcher}, master_account_{config.get_master_account()}, context_{context}, shared_{dispatcher, settings},
      accounts_{create_accounts<decltype(accounts_)>(config)}, requests_{create_requests<decltype(requests_)>(config, shared_)},
      rest_{create_rest<decltype(rest_)>(*this, context_, stream_id_, shared_, accounts_, master_account_)},
      order_entry_{create_order_entry<decltype(order_entry_)>(*this, context_, stream_id_, accounts_, shared_, requests_)},
      drop_copy_{create_drop_copy<decltype(drop_copy_)>(*this, context_, stream_id_, accounts_, shared_, requests_)} {
}

// server::Handler

void Controller::operator()(Trace<Start> const &event) {
  log::info("Starting..."sv);
  assert(std::empty(market_data_));
  dispatch(event);
}

void Controller::operator()(Trace<Stop> const &event) {
  log::info("Stopping..."sv);
  dispatch(event);
}

void Controller::operator()(Trace<Timer> const &event) {
  dispatch(event);
}

void Controller::operator()(Trace<Connected> const &, [[maybe_unused]] uint8_t user_id) {
}

void Controller::operator()(Trace<Disconnected> const &, [[maybe_unused]] uint8_t user_id) {
}

void Controller::operator()(Trace<Subscribe> const &event) {
  auto &[trace_info, subscribe] = event;
  std::vector<Symbol> symbols;
  for (auto &item : subscribe.symbols) {
    if (shared_.all_symbols.emplace(item).second) {
      symbols.emplace_back(item);
    } else {
      log::warn(R"(*** DUPLICATE SUBSCRIPTION *** (symbol="{}")"sv, item);
    }
  }
  auto symbols_update = SymbolsUpdate{
      .symbols = symbols,
  };
  create_trace_and_dispatch_2(trace_info, symbols_update);
}

void Controller::operator()(Trace<Control> const &event) {
  auto &[trace_info, control] = event;
  switch (control.action) {
    using enum Action;
    case UNDEFINED:
      assert(false);
      break;
    case ENABLE:
      dispatcher_(State::ENABLED);
      break;
    case DISABLE:
      dispatcher_(State::DISABLED);
      break;
  }
}

void Controller::operator()(Trace<server::oms::OrderActionBatchBegin> const &event) {
  dispatch_order_action<void>(event);
}

void Controller::operator()(Trace<server::oms::OrderActionBatchCommit> const &event) {
  dispatch_order_action<void>(event);
}

void Controller::operator()(Trace<server::oms::OrderActionBatchRollback> const &event) {
  dispatch_order_action<void>(event);
}

uint16_t Controller::operator()(
    Trace<CreateOrder> const &event, server::oms::Order const &order, server::oms::RefData const &ref_data, std::string_view const &request_id) {
  return dispatch_order_action<uint16_t>(event, order, ref_data, request_id);
}

uint16_t Controller::operator()(
    Trace<ModifyOrder> const &event,
    server::oms::Order const &order,
    server::oms::RefData const &ref_data,
    std::string_view const &request_id,
    std::string_view const &previous_request_id) {
  return dispatch_order_action<uint16_t>(event, order, ref_data, request_id, previous_request_id);
}

uint16_t Controller::operator()(
    Trace<CancelOrder> const &event,
    server::oms::Order const &order,
    server::oms::RefData const &ref_data,
    std::string_view const &request_id,
    std::string_view const &previous_request_id) {
  return dispatch_order_action<uint16_t>(event, order, ref_data, request_id, previous_request_id);
}

uint16_t Controller::operator()(Trace<CancelAllOrders> const &event, std::string_view const &request_id) {
  return dispatch_order_action<uint16_t>(event, request_id);
}

uint16_t Controller::operator()(Trace<MassQuote> const &) {
  throw server::oms::NotSupported{"not supported"sv};
}

uint16_t Controller::operator()(Trace<CancelQuotes> const &) {
  throw server::oms::NotSupported{"not supported"sv};
}

void Controller::operator()(metrics::Writer &writer) const {
  dispatch_helper(*this, writer);
}

// RestUsdM::Handler + RestCoinM::Handler

void Controller::operator()(Trace<SymbolsUpdate> const &event) {
  auto &[trace_info, symbols_update] = event;
  auto [size, start_from] = shared_.symbols(symbols_update.symbols);
  ensure_symbol_slices(trace_info, size);
  for (auto &iter : market_data_) {
    (*iter).subscribe(start_from);
  }
}

// utilities

void Controller::ensure_symbol_slices(TraceInfo const &trace_info, size_t size) {
  auto helper = [&](std::unique_ptr<server::MarketDataStream> &&market_data) {
    Start start;
    create_trace_and_dispatch(dynamic_cast<server::Stream &>(*market_data), trace_info, start);
    market_data_.emplace_back(std::move(market_data));
  };
  while (std::size(market_data_) < size) {
    log::debug("Create market-data (user-stream)"sv);
    switch (shared_.api.type) {
      using enum API::Type;
      case COIN_M: {
        auto obj = std::make_unique<MarketDataCoinM>(*this, context_, ++stream_id_, shared_, std::size(market_data_));
        helper(std::move(obj));
        break;
      }
      case USD_M: {
        auto obj = std::make_unique<MarketDataUsdM>(*this, context_, ++stream_id_, shared_, std::size(market_data_));
        helper(std::move(obj));
        break;
      }
    }
  }
}

template <typename... Args>
void Controller::dispatch(Args &&...args) {
  dispatch_helper(*this, std::forward<Args>(args)...);
}

template <typename... Args>
void Controller::dispatch_helper(auto &self, Args &&...args) {
  auto helper = [&](auto &target) {
    if constexpr (std::is_const_v<decltype(self)>) {
      dynamic_cast<server::Stream const &>(target)(std::forward<Args>(args)...);
    } else {
      dynamic_cast<server::Stream &>(target)(std::forward<Args>(args)...);
    }
  };
  helper(*self.rest_);
  for (auto &[_, order_entry] : self.order_entry_) {
    for (auto &item : order_entry.order_entry) {
      helper(*item);
    }
  }
  for (auto &[_, drop_copy] : self.drop_copy_) {
    if (static_cast<bool>(drop_copy)) {
      helper(*drop_copy);
    }
  }
  for (auto &iter : self.market_data_) {
    helper(*iter);
  }
}

template <typename Res, typename T, typename... Args>
Res Controller::dispatch_order_action(Trace<T> const &event, Args &&...args) {
  auto &[trace_info, value] = event;
  auto iter = order_entry_.find(value.account);
  if (iter == std::end(order_entry_)) [[unlikely]] {
    throw RuntimeError(R"(Unknown account="{}")"sv, value.account);
  }
  return (*iter).second.get_next()(event, args...);
}

// OrderEntryRR

Controller::OrderEntryRR::OrderEntryRR(std::vector<std::unique_ptr<server::OrderActionStream>> &&order_entry) : order_entry{std::move(order_entry)} {
}

server::OrderActionStream &Controller::OrderEntryRR::get_next() {
  auto length = std::size(order_entry);
  for (size_t offset = 0; offset < length; ++offset) {
    auto index = (index_ + offset) % length;
    auto &obj = *(order_entry[index]);
    if (!obj.ready()) {
      continue;
    }
    index_ = (index + 1) % length;
    return obj;
  }
  throw server::oms::NotReady{"get_next"sv};
}

}  // namespace gateway
}  // namespace phemex_futures
}  // namespace roq
