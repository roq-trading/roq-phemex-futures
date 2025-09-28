/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/phemex/json/map.hpp"

using namespace std::literals;

namespace roq {

namespace {
template <typename... Args>
using Helper = detail::MapHelper<Args...>;
}

// phemex::json => roq

// phemex::json::Action => roq::MarginMode

template <>
template <>
constexpr Helper<phemex::json::Action>::operator std::optional<roq::UpdateType>() const {
  switch (std::get<0>(args_)) {
    using enum phemex::json::Action::type_t;
    case UNDEFINED_INTERNAL:
      return roq::UpdateType::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::UpdateType::UNDEFINED;
    case SNAPSHOT:
      return roq::UpdateType::SNAPSHOT;
    case UPDATE:
      return roq::UpdateType::INCREMENTAL;
  }
  return {};
}

static_assert(Helper{phemex::json::Action{phemex::json::Action::UNDEFINED_INTERNAL}} == roq::UpdateType::UNDEFINED);
static_assert(Helper{phemex::json::Action{phemex::json::Action::SNAPSHOT}} == roq::UpdateType::SNAPSHOT);
static_assert(Helper{phemex::json::Action{phemex::json::Action::UPDATE}} == roq::UpdateType::INCREMENTAL);

template <>
template <>
std::optional<roq::UpdateType> Map<phemex::json::Action>::helper() const {
  return Helper{args_};
}

// phemex::json::AssetMode => roq::MarginMode

template <>
template <>
constexpr Helper<phemex::json::AssetMode>::operator std::optional<roq::MarginMode>() const {
  switch (std::get<0>(args_)) {
    using enum phemex::json::AssetMode::type_t;
    case UNDEFINED_INTERNAL:
      return roq::MarginMode::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::MarginMode::UNDEFINED;
    case SINGLE:
      return roq::MarginMode::ISOLATED;
    case UNION:
      return roq::MarginMode::CROSS;
    case MULTI_ASSETS:
      return roq::MarginMode::CROSS;  // ???
  }
  return {};
}

static_assert(Helper{phemex::json::AssetMode{phemex::json::AssetMode::UNDEFINED_INTERNAL}} == roq::MarginMode::UNDEFINED);
static_assert(Helper{phemex::json::AssetMode{phemex::json::AssetMode::SINGLE}} == roq::MarginMode::ISOLATED);
static_assert(Helper{phemex::json::AssetMode{phemex::json::AssetMode::UNION}} == roq::MarginMode::CROSS);
static_assert(Helper{phemex::json::AssetMode{phemex::json::AssetMode::MULTI_ASSETS}} == roq::MarginMode::CROSS);

template <>
template <>
std::optional<roq::MarginMode> Map<phemex::json::AssetMode>::helper() const {
  return Helper{args_};
}

// {phemex::json::Category, phemex::json::FuturesType} => roq::SecurityType

template <>
template <>
constexpr Helper<phemex::json::Category, phemex::json::FuturesType>::operator std::optional<roq::SecurityType>() const {
  switch (std::get<0>(args_)) {
    using enum phemex::json::Category::type_t;
    case UNDEFINED_INTERNAL:
      return roq::SecurityType::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::SecurityType::UNDEFINED;
    case SPOT:
      return roq::SecurityType::SPOT;
    case MARGIN:
      return roq::SecurityType::SPOT;
    case USDT_FUTURES:
    case USDC_FUTURES:
    case COIN_FUTURES:
      switch (std::get<1>(args_)) {
        using enum phemex::json::FuturesType::type_t;
        case UNDEFINED_INTERNAL:
          return roq::SecurityType::UNDEFINED;
        case UNKNOWN_INTERNAL:
          return roq::SecurityType::UNDEFINED;
        case PERPETUAL:
          return roq::SecurityType::SWAP;
        case DELIVERY:
          return roq::SecurityType::FUTURES;
      }
      break;
  }
  return {};
}

static_assert(
    Helper{phemex::json::Category{phemex::json::Category::UNDEFINED_INTERNAL}, phemex::json::FuturesType{phemex::json::FuturesType::UNDEFINED_INTERNAL}} ==
    roq::SecurityType::UNDEFINED);
static_assert(
    Helper{phemex::json::Category{phemex::json::Category::SPOT}, phemex::json::FuturesType{phemex::json::FuturesType::UNDEFINED_INTERNAL}} ==
    roq::SecurityType::SPOT);
static_assert(
    Helper{phemex::json::Category{phemex::json::Category::MARGIN}, phemex::json::FuturesType{phemex::json::FuturesType::UNDEFINED_INTERNAL}} ==
    roq::SecurityType::SPOT);

static_assert(
    Helper{phemex::json::Category{phemex::json::Category::USDT_FUTURES}, phemex::json::FuturesType{phemex::json::FuturesType::UNDEFINED_INTERNAL}} ==
    roq::SecurityType::UNDEFINED);
static_assert(
    Helper{phemex::json::Category{phemex::json::Category::USDT_FUTURES}, phemex::json::FuturesType{phemex::json::FuturesType::UNKNOWN_INTERNAL}} ==
    roq::SecurityType::UNDEFINED);
static_assert(
    Helper{phemex::json::Category{phemex::json::Category::USDT_FUTURES}, phemex::json::FuturesType{phemex::json::FuturesType::PERPETUAL}} ==
    roq::SecurityType::SWAP);
static_assert(
    Helper{phemex::json::Category{phemex::json::Category::USDT_FUTURES}, phemex::json::FuturesType{phemex::json::FuturesType::DELIVERY}} ==
    roq::SecurityType::FUTURES);

static_assert(
    Helper{phemex::json::Category{phemex::json::Category::USDC_FUTURES}, phemex::json::FuturesType{phemex::json::FuturesType::UNDEFINED_INTERNAL}} ==
    roq::SecurityType::UNDEFINED);
static_assert(
    Helper{phemex::json::Category{phemex::json::Category::USDC_FUTURES}, phemex::json::FuturesType{phemex::json::FuturesType::UNKNOWN_INTERNAL}} ==
    roq::SecurityType::UNDEFINED);
static_assert(
    Helper{phemex::json::Category{phemex::json::Category::USDC_FUTURES}, phemex::json::FuturesType{phemex::json::FuturesType::PERPETUAL}} ==
    roq::SecurityType::SWAP);
static_assert(
    Helper{phemex::json::Category{phemex::json::Category::USDC_FUTURES}, phemex::json::FuturesType{phemex::json::FuturesType::DELIVERY}} ==
    roq::SecurityType::FUTURES);

static_assert(
    Helper{phemex::json::Category{phemex::json::Category::COIN_FUTURES}, phemex::json::FuturesType{phemex::json::FuturesType::UNDEFINED_INTERNAL}} ==
    roq::SecurityType::UNDEFINED);
static_assert(
    Helper{phemex::json::Category{phemex::json::Category::COIN_FUTURES}, phemex::json::FuturesType{phemex::json::FuturesType::UNKNOWN_INTERNAL}} ==
    roq::SecurityType::UNDEFINED);
static_assert(
    Helper{phemex::json::Category{phemex::json::Category::COIN_FUTURES}, phemex::json::FuturesType{phemex::json::FuturesType::PERPETUAL}} ==
    roq::SecurityType::SWAP);
static_assert(
    Helper{phemex::json::Category{phemex::json::Category::COIN_FUTURES}, phemex::json::FuturesType{phemex::json::FuturesType::DELIVERY}} ==
    roq::SecurityType::FUTURES);

template <>
template <>
std::optional<roq::SecurityType> Map<phemex::json::Category, phemex::json::FuturesType>::helper() const {
  return Helper{args_};
}

// phemex::json::MarginMode => roq::MarginMode

template <>
template <>
constexpr Helper<phemex::json::MarginMode>::operator std::optional<roq::MarginMode>() const {
  switch (std::get<0>(args_)) {
    using enum phemex::json::MarginMode::type_t;
    case UNDEFINED_INTERNAL:
      return roq::MarginMode::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::MarginMode::UNDEFINED;
    case CROSSED:
      return roq::MarginMode::CROSS;
    case ISOLATED:
      return roq::MarginMode::ISOLATED;
  }
  return {};
}

static_assert(Helper{phemex::json::MarginMode{phemex::json::MarginMode::UNDEFINED_INTERNAL}} == roq::MarginMode::UNDEFINED);
static_assert(Helper{phemex::json::MarginMode{phemex::json::MarginMode::CROSSED}} == roq::MarginMode::CROSS);
static_assert(Helper{phemex::json::MarginMode{phemex::json::MarginMode::ISOLATED}} == roq::MarginMode::ISOLATED);

template <>
template <>
std::optional<roq::MarginMode> Map<phemex::json::MarginMode>::helper() const {
  return Helper{args_};
}

// phemex::json::MessageType => roq::UpdateType

template <>
template <>
constexpr Helper<phemex::json::MessageType>::operator std::optional<roq::UpdateType>() const {
  switch (std::get<0>(args_)) {
    using enum phemex::json::MessageType::type_t;
    case UNDEFINED_INTERNAL:
      return roq::UpdateType::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::UpdateType::UNDEFINED;
    case SNAPSHOT:
      return roq::UpdateType::SNAPSHOT;
    case INCREMENTAL:
      return roq::UpdateType::INCREMENTAL;
  }
  return {};
}

static_assert(Helper{phemex::json::MessageType{phemex::json::MessageType::UNDEFINED_INTERNAL}} == roq::UpdateType::UNDEFINED);
static_assert(Helper{phemex::json::MessageType{phemex::json::MessageType::SNAPSHOT}} == roq::UpdateType::SNAPSHOT);
static_assert(Helper{phemex::json::MessageType{phemex::json::MessageType::INCREMENTAL}} == roq::UpdateType::INCREMENTAL);

template <>
template <>
std::optional<roq::UpdateType> Map<phemex::json::MessageType>::helper() const {
  return Helper{args_};
}

// phemex::json::OrderStatus => roq::OrderStatus

template <>
template <>
constexpr Helper<phemex::json::OrderStatus>::operator std::optional<roq::OrderStatus>() const {
  switch (std::get<0>(args_)) {
    using enum phemex::json::OrderStatus::type_t;
    case UNDEFINED_INTERNAL:
      return roq::OrderStatus::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::OrderStatus::UNDEFINED;
    case LIVE:
      return roq::OrderStatus::WORKING;
    case NEW:
      return roq::OrderStatus::WORKING;
    case PARTIALLY_FILLED:
      return roq::OrderStatus::WORKING;
    case FILLED:
      return roq::OrderStatus::COMPLETED;
    case CANCELLED:
      return roq::OrderStatus::CANCELED;
  }
  return {};
}

static_assert(Helper{phemex::json::OrderStatus{phemex::json::OrderStatus::UNDEFINED_INTERNAL}} == roq::OrderStatus::UNDEFINED);
static_assert(Helper{phemex::json::OrderStatus{phemex::json::OrderStatus::LIVE}} == roq::OrderStatus::WORKING);
static_assert(Helper{phemex::json::OrderStatus{phemex::json::OrderStatus::NEW}} == roq::OrderStatus::WORKING);
static_assert(Helper{phemex::json::OrderStatus{phemex::json::OrderStatus::PARTIALLY_FILLED}} == roq::OrderStatus::WORKING);
static_assert(Helper{phemex::json::OrderStatus{phemex::json::OrderStatus::FILLED}} == roq::OrderStatus::COMPLETED);
static_assert(Helper{phemex::json::OrderStatus{phemex::json::OrderStatus::CANCELLED}} == roq::OrderStatus::CANCELED);

template <>
template <>
std::optional<roq::OrderStatus> Map<phemex::json::OrderStatus>::helper() const {
  return Helper{args_};
}

// phemex::json::OrderType => roq::OrderType

template <>
template <>
constexpr Helper<phemex::json::OrderType>::operator std::optional<roq::OrderType>() const {
  switch (std::get<0>(args_)) {
    using enum phemex::json::OrderType::type_t;
    case UNDEFINED_INTERNAL:
      return roq::OrderType::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::OrderType::UNDEFINED;
    case MARKET:
      return roq::OrderType::MARKET;
    case LIMIT:
      return roq::OrderType::LIMIT;
  }
  return {};
}

static_assert(Helper{phemex::json::OrderType{phemex::json::OrderType::UNDEFINED_INTERNAL}} == roq::OrderType::UNDEFINED);
static_assert(Helper{phemex::json::OrderType{phemex::json::OrderType::MARKET}} == roq::OrderType::MARKET);
static_assert(Helper{phemex::json::OrderType{phemex::json::OrderType::LIMIT}} == roq::OrderType::LIMIT);

template <>
template <>
std::optional<roq::OrderType> Map<phemex::json::OrderType>::helper() const {
  return Helper{args_};
}

// {phemex::json::PosSide, phemex::json::Side} => roq::PositionEffect

template <>
template <>
constexpr Helper<phemex::json::PosSide, phemex::json::Side>::operator std::optional<roq::PositionEffect>() const {
  switch (std::get<0>(args_)) {
    using enum phemex::json::PosSide::type_t;
    case UNDEFINED_INTERNAL:
      return roq::PositionEffect::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::PositionEffect::UNDEFINED;
    case LONG:
      switch (std::get<1>(args_)) {
        using enum phemex::json::Side::type_t;
        case UNDEFINED_INTERNAL:
          return roq::PositionEffect::UNDEFINED;
        case UNKNOWN_INTERNAL:
          return roq::PositionEffect::UNDEFINED;
        case BUY:
          return roq::PositionEffect::OPEN;
        case SELL:
          return roq::PositionEffect::CLOSE;
      }
      break;
    case SHORT:
      switch (std::get<1>(args_)) {
        using enum phemex::json::Side::type_t;
        case UNDEFINED_INTERNAL:
          return roq::PositionEffect::UNDEFINED;
        case UNKNOWN_INTERNAL:
          return roq::PositionEffect::UNDEFINED;
        case BUY:
          return roq::PositionEffect::CLOSE;
        case SELL:
          return roq::PositionEffect::OPEN;
      }
      break;
    case NET:
      return roq::PositionEffect::UNDEFINED;
  }
  return {};
}

static_assert(
    Helper{phemex::json::PosSide{phemex::json::PosSide::UNDEFINED_INTERNAL}, phemex::json::Side{phemex::json::Side::UNDEFINED_INTERNAL}} ==
    roq::PositionEffect::UNDEFINED);
static_assert(Helper{phemex::json::PosSide{phemex::json::PosSide::LONG}, phemex::json::Side{phemex::json::Side::BUY}} == roq::PositionEffect::OPEN);
static_assert(Helper{phemex::json::PosSide{phemex::json::PosSide::LONG}, phemex::json::Side{phemex::json::Side::SELL}} == roq::PositionEffect::CLOSE);
static_assert(Helper{phemex::json::PosSide{phemex::json::PosSide::SHORT}, phemex::json::Side{phemex::json::Side::BUY}} == roq::PositionEffect::CLOSE);
static_assert(Helper{phemex::json::PosSide{phemex::json::PosSide::SHORT}, phemex::json::Side{phemex::json::Side::SELL}} == roq::PositionEffect::OPEN);
static_assert(Helper{phemex::json::PosSide{phemex::json::PosSide::NET}, phemex::json::Side{phemex::json::Side::BUY}} == roq::PositionEffect::UNDEFINED);
static_assert(Helper{phemex::json::PosSide{phemex::json::PosSide::NET}, phemex::json::Side{phemex::json::Side::SELL}} == roq::PositionEffect::UNDEFINED);

template <>
template <>
std::optional<roq::PositionEffect> Map<phemex::json::PosSide, phemex::json::Side>::helper() const {
  return Helper{args_};
}

// phemex::json::Side => roq::Side

template <>
template <>
constexpr Helper<phemex::json::Side>::operator std::optional<roq::Side>() const {
  switch (std::get<0>(args_)) {
    using enum phemex::json::Side::type_t;
    case UNDEFINED_INTERNAL:
      return roq::Side::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::Side::UNDEFINED;
    case BUY:
      return roq::Side::BUY;
    case SELL:
      return roq::Side::SELL;
  }
  return {};
}

static_assert(Helper{phemex::json::Side{phemex::json::Side::UNDEFINED_INTERNAL}} == roq::Side::UNDEFINED);
static_assert(Helper{phemex::json::Side{phemex::json::Side::BUY}} == roq::Side::BUY);
static_assert(Helper{phemex::json::Side{phemex::json::Side::SELL}} == roq::Side::SELL);

template <>
template <>
std::optional<roq::Side> Map<phemex::json::Side>::helper() const {
  return Helper{args_};
}

// phemex::json::TimeInForce => roq::TimeInForce

template <>
template <>
constexpr Helper<phemex::json::TimeInForce>::operator std::optional<roq::TimeInForce>() const {
  switch (std::get<0>(args_)) {
    using enum phemex::json::TimeInForce::type_t;
    case UNDEFINED_INTERNAL:
      return roq::TimeInForce::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::TimeInForce::UNDEFINED;
    case IOC:
      return roq::TimeInForce::IOC;
    case FOK:
      return roq::TimeInForce::FOK;
    case GTC:
      return roq::TimeInForce::GTC;
    case POST_ONLY:
      return roq::TimeInForce::GTC;
  }
  return {};
}

static_assert(Helper{phemex::json::TimeInForce{phemex::json::TimeInForce::UNDEFINED_INTERNAL}} == roq::TimeInForce::UNDEFINED);
static_assert(Helper{phemex::json::TimeInForce{phemex::json::TimeInForce::IOC}} == roq::TimeInForce::IOC);
static_assert(Helper{phemex::json::TimeInForce{phemex::json::TimeInForce::FOK}} == roq::TimeInForce::FOK);
static_assert(Helper{phemex::json::TimeInForce{phemex::json::TimeInForce::GTC}} == roq::TimeInForce::GTC);
static_assert(Helper{phemex::json::TimeInForce{phemex::json::TimeInForce::POST_ONLY}} == roq::TimeInForce::GTC);

template <>
template <>
std::optional<roq::TimeInForce> Map<phemex::json::TimeInForce>::helper() const {
  return Helper{args_};
}

// phemex::json::TradeScope => roq::Liquidity

template <>
template <>
constexpr Helper<phemex::json::TradeScope>::operator std::optional<roq::Liquidity>() const {
  switch (std::get<0>(args_)) {
    using enum phemex::json::TradeScope::type_t;
    case UNDEFINED_INTERNAL:
      return roq::Liquidity::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::Liquidity::UNDEFINED;
    case TAKER:
      return roq::Liquidity::TAKER;
    case MAKER:
      return roq::Liquidity::MAKER;
  }
  return {};
}

static_assert(Helper{phemex::json::TradeScope{phemex::json::TradeScope::UNDEFINED_INTERNAL}} == roq::Liquidity::UNDEFINED);
static_assert(Helper{phemex::json::TradeScope{phemex::json::TradeScope::TAKER}} == roq::Liquidity::TAKER);
static_assert(Helper{phemex::json::TradeScope{phemex::json::TradeScope::MAKER}} == roq::Liquidity::MAKER);

template <>
template <>
std::optional<roq::Liquidity> Map<phemex::json::TradeScope>::helper() const {
  return Helper{args_};
}

// phemex::json::TradeSide => roq::PositionEffect

template <>
template <>
constexpr Helper<phemex::json::TradeSide>::operator std::optional<roq::PositionEffect>() const {
  switch (std::get<0>(args_)) {
    using enum phemex::json::TradeSide::type_t;
    case UNDEFINED_INTERNAL:
      return roq::PositionEffect::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::PositionEffect::UNDEFINED;
    case CLOSE:
      return roq::PositionEffect::CLOSE;
    case OPEN:
      return roq::PositionEffect::OPEN;
    case REDUCE_CLOSE_LONG:
      return roq::PositionEffect::CLOSE;
    case REDUCE_CLOSE_SHORT:
      return roq::PositionEffect::CLOSE;
    case BURST_CLOSE_LONG:
      return roq::PositionEffect::CLOSE;
    case BURST_CLOSE_SHORT:
      return roq::PositionEffect::CLOSE;
    case OFFSET_CLOSE_LONG:
      return roq::PositionEffect::CLOSE;
    case OFFSET_CLOSE_SHORT:
      return roq::PositionEffect::CLOSE;
    case DELIVERY_CLOSE_LONG:
      return roq::PositionEffect::CLOSE;
    case DELIVERY_CLOSE_SHORT:
      return roq::PositionEffect::CLOSE;
    case DTE_SYS_ADL_CLOSE_LONG:
      return roq::PositionEffect::CLOSE;
    case DTE_SYS_ADL_CLOSE_SHORT:
      return roq::PositionEffect::CLOSE;
    case BUY_SINGLE:
      return roq::PositionEffect::UNDEFINED;
    case SELL_SINGLE:
      return roq::PositionEffect::UNDEFINED;
    case REDUCE_BUY_SINGLE:
      return roq::PositionEffect::UNDEFINED;
    case REDUCE_SELL_SINGLE:
      return roq::PositionEffect::UNDEFINED;
    case BURST_BUY_SINGLE:
      return roq::PositionEffect::UNDEFINED;
    case BURST_SELL_SINGLE:
      return roq::PositionEffect::UNDEFINED;
    case DELIVERY_SELL_SINGLE:
      return roq::PositionEffect::UNDEFINED;
    case DELIVERY_BUY_SINGLE:
      return roq::PositionEffect::UNDEFINED;
    case DTE_SYS_ADL_BUY_IN_SINGLE_SIDE_MODE:
      return roq::PositionEffect::UNDEFINED;
    case DTE_SYS_ADL_SELL_IN_SINGLE_SIDE_MODE:
      return roq::PositionEffect::UNDEFINED;
    case OPEN_SHORT:
      return roq::PositionEffect::OPEN;
    case OPEN_LONG:
      return roq::PositionEffect::OPEN;
    case CLOSE_SHORT:
      return roq::PositionEffect::CLOSE;
    case CLOSE_LONG:
      return roq::PositionEffect::CLOSE;
  }
  return {};
}

static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::UNDEFINED_INTERNAL}} == roq::PositionEffect::UNDEFINED);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::CLOSE}} == roq::PositionEffect::CLOSE);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::OPEN}} == roq::PositionEffect::OPEN);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::REDUCE_CLOSE_LONG}} == roq::PositionEffect::CLOSE);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::REDUCE_CLOSE_SHORT}} == roq::PositionEffect::CLOSE);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::BURST_CLOSE_LONG}} == roq::PositionEffect::CLOSE);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::BURST_CLOSE_SHORT}} == roq::PositionEffect::CLOSE);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::OFFSET_CLOSE_LONG}} == roq::PositionEffect::CLOSE);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::OFFSET_CLOSE_SHORT}} == roq::PositionEffect::CLOSE);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::DELIVERY_CLOSE_LONG}} == roq::PositionEffect::CLOSE);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::DELIVERY_CLOSE_SHORT}} == roq::PositionEffect::CLOSE);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::DTE_SYS_ADL_CLOSE_LONG}} == roq::PositionEffect::CLOSE);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::DTE_SYS_ADL_CLOSE_SHORT}} == roq::PositionEffect::CLOSE);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::BUY_SINGLE}} == roq::PositionEffect::UNDEFINED);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::SELL_SINGLE}} == roq::PositionEffect::UNDEFINED);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::REDUCE_BUY_SINGLE}} == roq::PositionEffect::UNDEFINED);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::REDUCE_SELL_SINGLE}} == roq::PositionEffect::UNDEFINED);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::BURST_BUY_SINGLE}} == roq::PositionEffect::UNDEFINED);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::BURST_SELL_SINGLE}} == roq::PositionEffect::UNDEFINED);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::DELIVERY_SELL_SINGLE}} == roq::PositionEffect::UNDEFINED);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::DELIVERY_BUY_SINGLE}} == roq::PositionEffect::UNDEFINED);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::DTE_SYS_ADL_BUY_IN_SINGLE_SIDE_MODE}} == roq::PositionEffect::UNDEFINED);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::DTE_SYS_ADL_SELL_IN_SINGLE_SIDE_MODE}} == roq::PositionEffect::UNDEFINED);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::OPEN_SHORT}} == roq::PositionEffect::OPEN);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::CLOSE_LONG}} == roq::PositionEffect::CLOSE);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::OPEN_SHORT}} == roq::PositionEffect::OPEN);
static_assert(Helper{phemex::json::TradeSide{phemex::json::TradeSide::CLOSE_LONG}} == roq::PositionEffect::CLOSE);

template <>
template <>
std::optional<roq::PositionEffect> Map<phemex::json::TradeSide>::helper() const {
  return Helper{args_};
}

// phemex::json::TradingStatus => roq::TradingStatus

template <>
template <>
constexpr Helper<phemex::json::TradingStatus>::operator std::optional<roq::TradingStatus>() const {
  switch (std::get<0>(args_)) {
    using enum phemex::json::TradingStatus::type_t;
    case UNDEFINED_INTERNAL:
      return roq::TradingStatus::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::TradingStatus::UNDEFINED;
    case LISTED:
      return roq::TradingStatus::UNDEFINED;
    case ONLINE:
      return roq::TradingStatus::OPEN;
    case LIMIT_OPEN:
      return roq::TradingStatus::PRE_OPEN;
    case OFFLINE:
      return roq::TradingStatus::CLOSE;
    case RESTRICTED_API:
      return roq::TradingStatus::HALT;
  }
  return {};
}

static_assert(Helper{phemex::json::TradingStatus{phemex::json::TradingStatus::UNDEFINED_INTERNAL}} == roq::TradingStatus::UNDEFINED);
static_assert(Helper{phemex::json::TradingStatus{phemex::json::TradingStatus::LISTED}} == roq::TradingStatus::UNDEFINED);
static_assert(Helper{phemex::json::TradingStatus{phemex::json::TradingStatus::ONLINE}} == roq::TradingStatus::OPEN);
static_assert(Helper{phemex::json::TradingStatus{phemex::json::TradingStatus::LIMIT_OPEN}} == roq::TradingStatus::PRE_OPEN);
static_assert(Helper{phemex::json::TradingStatus{phemex::json::TradingStatus::OFFLINE}} == roq::TradingStatus::CLOSE);
static_assert(Helper{phemex::json::TradingStatus{phemex::json::TradingStatus::RESTRICTED_API}} == roq::TradingStatus::HALT);

template <>
template <>
std::optional<roq::TradingStatus> Map<phemex::json::TradingStatus>::helper() const {
  return Helper{args_};
}

// roq => phemex::json

// roq::MarginMode => phemex::json::MarginMode

template <>
template <>
constexpr Helper<roq::MarginMode>::operator std::optional<phemex::json::MarginMode>() const {
  switch (std::get<0>(args_)) {
    using enum roq::MarginMode;
    case UNDEFINED:
      return phemex::json::MarginMode::UNDEFINED_INTERNAL;
    case CROSS:
      return phemex::json::MarginMode::CROSSED;
    case ISOLATED:
      return phemex::json::MarginMode::ISOLATED;
    case PORTFOLIO:
      return phemex::json::MarginMode::UNDEFINED_INTERNAL;
  }
  return {};
}

static_assert(Helper{roq::MarginMode::UNDEFINED} == phemex::json::MarginMode{phemex::json::MarginMode::UNDEFINED_INTERNAL});
static_assert(Helper{roq::MarginMode::CROSS} == phemex::json::MarginMode{phemex::json::MarginMode::CROSSED});
static_assert(Helper{roq::MarginMode::ISOLATED} == phemex::json::MarginMode{phemex::json::MarginMode::ISOLATED});
static_assert(Helper{roq::MarginMode::PORTFOLIO} == phemex::json::MarginMode{phemex::json::MarginMode::UNDEFINED_INTERNAL});

template <>
template <>
std::optional<phemex::json::MarginMode> Map<roq::MarginMode>::helper() const {
  return Helper{args_};
}

// roq::OrderType => phemex::json::OrderType

template <>
template <>
constexpr Helper<roq::OrderType>::operator std::optional<phemex::json::OrderType>() const {
  switch (std::get<0>(args_)) {
    using enum roq::OrderType;
    case UNDEFINED:
      return phemex::json::OrderType::UNDEFINED_INTERNAL;
    case MARKET:
      return phemex::json::OrderType::MARKET;
    case LIMIT:
      return phemex::json::OrderType::LIMIT;
  }
  return {};
}

static_assert(Helper{roq::OrderType::UNDEFINED} == phemex::json::OrderType{phemex::json::OrderType::UNDEFINED_INTERNAL});
static_assert(Helper{roq::OrderType::MARKET} == phemex::json::OrderType{phemex::json::OrderType::MARKET});
static_assert(Helper{roq::OrderType::LIMIT} == phemex::json::OrderType{phemex::json::OrderType::LIMIT});

template <>
template <>
std::optional<phemex::json::OrderType> Map<roq::OrderType>::helper() const {
  return Helper{args_};
}

// {roq::PositionEffect, roq::Side} => phemex::json::PosSide

template <>
template <>
constexpr Helper<roq::PositionEffect, roq::Side>::operator std::optional<phemex::json::PosSide>() const {
  switch (std::get<0>(args_)) {
    using enum roq::PositionEffect;
    case UNDEFINED:
      return phemex::json::PosSide::UNDEFINED_INTERNAL;
    case OPEN:
      switch (std::get<1>(args_)) {
        using enum roq::Side;
        case UNDEFINED:
          return phemex::json::PosSide::UNDEFINED_INTERNAL;
        case BUY:
          return phemex::json::PosSide::LONG;
        case SELL:
          return phemex::json::PosSide::SHORT;
      }
      break;
    case CLOSE:
      switch (std::get<1>(args_)) {
        using enum roq::Side;
        case UNDEFINED:
          return phemex::json::PosSide::UNDEFINED_INTERNAL;
        case BUY:
          return phemex::json::PosSide::SHORT;
        case SELL:
          return phemex::json::PosSide::LONG;
      }
      break;
  }
  return {};
}

static_assert(Helper{roq::PositionEffect::UNDEFINED, roq::Side::UNDEFINED} == phemex::json::PosSide{phemex::json::PosSide::UNDEFINED_INTERNAL});
static_assert(Helper{roq::PositionEffect::OPEN, roq::Side::BUY} == phemex::json::PosSide{phemex::json::PosSide::LONG});
static_assert(Helper{roq::PositionEffect::OPEN, roq::Side::SELL} == phemex::json::PosSide{phemex::json::PosSide::SHORT});
static_assert(Helper{roq::PositionEffect::CLOSE, roq::Side::BUY} == phemex::json::PosSide{phemex::json::PosSide::SHORT});
static_assert(Helper{roq::PositionEffect::CLOSE, roq::Side::SELL} == phemex::json::PosSide{phemex::json::PosSide::LONG});

template <>
template <>
std::optional<phemex::json::PosSide> Map<roq::PositionEffect, roq::Side>::helper() const {
  return Helper{args_};
}

// roq::PositionEffect => phemex::json::TradeSide

template <>
template <>
constexpr Helper<roq::PositionEffect>::operator std::optional<phemex::json::TradeSide>() const {
  switch (std::get<0>(args_)) {
    using enum roq::PositionEffect;
    case UNDEFINED:
      return phemex::json::TradeSide::UNDEFINED_INTERNAL;
    case OPEN:
      return phemex::json::TradeSide::OPEN;
    case CLOSE:
      return phemex::json::TradeSide::CLOSE;
  }
  return {};
}

static_assert(Helper{roq::PositionEffect::UNDEFINED} == phemex::json::TradeSide{phemex::json::TradeSide::UNDEFINED_INTERNAL});
static_assert(Helper{roq::PositionEffect::OPEN} == phemex::json::TradeSide{phemex::json::TradeSide::OPEN});
static_assert(Helper{roq::PositionEffect::CLOSE} == phemex::json::TradeSide{phemex::json::TradeSide::CLOSE});

template <>
template <>
std::optional<phemex::json::TradeSide> Map<roq::PositionEffect>::helper() const {
  return Helper{args_};
}

// roq::Side => phemex::json::Side

template <>
template <>
constexpr Helper<roq::Side>::operator std::optional<phemex::json::Side>() const {
  switch (std::get<0>(args_)) {
    using enum roq::Side;
    case UNDEFINED:
      return phemex::json::Side::UNDEFINED_INTERNAL;
    case BUY:
      return phemex::json::Side::BUY;
    case SELL:
      return phemex::json::Side::SELL;
  }
  return {};
}

static_assert(Helper{roq::Side::UNDEFINED} == phemex::json::Side{phemex::json::Side::UNDEFINED_INTERNAL});
static_assert(Helper{roq::Side::BUY} == phemex::json::Side{phemex::json::Side::BUY});
static_assert(Helper{roq::Side::SELL} == phemex::json::Side{phemex::json::Side::SELL});

template <>
template <>
std::optional<phemex::json::Side> Map<roq::Side>::helper() const {
  return Helper{args_};
}

// roq::TimeInForce => phemex::json::Force

// POST_ONLY ???
template <>
template <>
constexpr Helper<roq::TimeInForce>::operator std::optional<phemex::json::TimeInForce>() const {
  switch (std::get<0>(args_)) {
    using enum roq::TimeInForce;
    case UNDEFINED:
      return phemex::json::TimeInForce::UNDEFINED_INTERNAL;
    case GFD:
      return phemex::json::TimeInForce::UNDEFINED_INTERNAL;
    case GTC:
      return phemex::json::TimeInForce::GTC;
    case OPG:
      return phemex::json::TimeInForce::UNDEFINED_INTERNAL;
    case IOC:
      return phemex::json::TimeInForce::IOC;
    case FOK:
      return phemex::json::TimeInForce::FOK;
    case GTX:
      return phemex::json::TimeInForce::UNDEFINED_INTERNAL;
    case GTD:
      return phemex::json::TimeInForce::UNDEFINED_INTERNAL;
    case AT_THE_CLOSE:
      return phemex::json::TimeInForce::UNDEFINED_INTERNAL;
    case GOOD_THROUGH_CROSSING:
      return phemex::json::TimeInForce::UNDEFINED_INTERNAL;
    case AT_CROSSING:
      return phemex::json::TimeInForce::UNDEFINED_INTERNAL;
    case GOOD_FOR_TIME:
      return phemex::json::TimeInForce::UNDEFINED_INTERNAL;
    case GFA:
      return phemex::json::TimeInForce::UNDEFINED_INTERNAL;
    case GFM:
      return phemex::json::TimeInForce::UNDEFINED_INTERNAL;
  }
  return {};
}

static_assert(Helper{roq::TimeInForce::UNDEFINED} == phemex::json::TimeInForce{phemex::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GFD} == phemex::json::TimeInForce{phemex::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GTC} == phemex::json::TimeInForce{phemex::json::TimeInForce::GTC});
static_assert(Helper{roq::TimeInForce::OPG} == phemex::json::TimeInForce{phemex::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::IOC} == phemex::json::TimeInForce{phemex::json::TimeInForce::IOC});
static_assert(Helper{roq::TimeInForce::FOK} == phemex::json::TimeInForce{phemex::json::TimeInForce::FOK});
static_assert(Helper{roq::TimeInForce::GTX} == phemex::json::TimeInForce{phemex::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GTD} == phemex::json::TimeInForce{phemex::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::AT_THE_CLOSE} == phemex::json::TimeInForce{phemex::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GOOD_THROUGH_CROSSING} == phemex::json::TimeInForce{phemex::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::AT_CROSSING} == phemex::json::TimeInForce{phemex::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GOOD_FOR_TIME} == phemex::json::TimeInForce{phemex::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GFA} == phemex::json::TimeInForce{phemex::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GFM} == phemex::json::TimeInForce{phemex::json::TimeInForce::UNDEFINED_INTERNAL});

template <>
template <>
std::optional<phemex::json::TimeInForce> Map<roq::TimeInForce>::helper() const {
  return Helper{args_};
}

}  // namespace roq
