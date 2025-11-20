/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/phemex_futures/json/map.hpp"

using namespace std::literals;

namespace roq {

namespace {
template <typename... Args>
using Helper = detail::MapHelper<Args...>;
}

// phemex_futures::json => roq

// phemex_futures::json::EventType => roq::UpdateType

template <>
template <>
constexpr Helper<phemex_futures::json::EventType>::operator std::optional<roq::UpdateType>() const {
  switch (std::get<0>(args_)) {
    using enum phemex_futures::json::EventType::type_t;
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

static_assert(Helper{phemex_futures::json::EventType{phemex_futures::json::EventType::UNDEFINED_INTERNAL}} == roq::UpdateType::UNDEFINED);
static_assert(Helper{phemex_futures::json::EventType{phemex_futures::json::EventType::SNAPSHOT}} == roq::UpdateType::SNAPSHOT);
static_assert(Helper{phemex_futures::json::EventType{phemex_futures::json::EventType::INCREMENTAL}} == roq::UpdateType::INCREMENTAL);

template <>
template <>
std::optional<roq::UpdateType> Map<phemex_futures::json::EventType>::helper() const {
  return Helper{args_};
}

// phemex_futures::json::LiquidityInd => roq::Liquidity

template <>
template <>
constexpr Helper<phemex_futures::json::LiquidityInd>::operator std::optional<roq::Liquidity>() const {
  switch (std::get<0>(args_)) {
    using enum phemex_futures::json::LiquidityInd::type_t;
    case UNDEFINED_INTERNAL:
      return roq::Liquidity::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::Liquidity::UNDEFINED;
    case ADDED_LIQUIDITY:
      return roq::Liquidity::MAKER;
    case REMOVED_LIQUIDITY:
      return roq::Liquidity::TAKER;
  }
  return {};
}

static_assert(Helper{phemex_futures::json::LiquidityInd{phemex_futures::json::LiquidityInd::UNDEFINED_INTERNAL}} == roq::Liquidity::UNDEFINED);
static_assert(Helper{phemex_futures::json::LiquidityInd{phemex_futures::json::LiquidityInd::ADDED_LIQUIDITY}} == roq::Liquidity::MAKER);
static_assert(Helper{phemex_futures::json::LiquidityInd{phemex_futures::json::LiquidityInd::REMOVED_LIQUIDITY}} == roq::Liquidity::TAKER);

template <>
template <>
std::optional<roq::Liquidity> Map<phemex_futures::json::LiquidityInd>::helper() const {
  return Helper{args_};
}

// phemex_futures::json::MessageType => roq::UpdateType

template <>
template <>
constexpr Helper<phemex_futures::json::MessageType>::operator std::optional<roq::UpdateType>() const {
  switch (std::get<0>(args_)) {
    using enum phemex_futures::json::MessageType::type_t;
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

static_assert(Helper{phemex_futures::json::MessageType{phemex_futures::json::MessageType::UNDEFINED_INTERNAL}} == roq::UpdateType::UNDEFINED);
static_assert(Helper{phemex_futures::json::MessageType{phemex_futures::json::MessageType::SNAPSHOT}} == roq::UpdateType::SNAPSHOT);
static_assert(Helper{phemex_futures::json::MessageType{phemex_futures::json::MessageType::INCREMENTAL}} == roq::UpdateType::INCREMENTAL);

template <>
template <>
std::optional<roq::UpdateType> Map<phemex_futures::json::MessageType>::helper() const {
  return Helper{args_};
}

// phemex_futures::json::OrderStatus => roq::OrderStatus

template <>
template <>
constexpr Helper<phemex_futures::json::OrderStatus>::operator std::optional<roq::OrderStatus>() const {
  switch (std::get<0>(args_)) {
    using enum phemex_futures::json::OrderStatus::type_t;
    case UNDEFINED_INTERNAL:
      return roq::OrderStatus::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::OrderStatus::UNDEFINED;
    case CREATED:
      return roq::OrderStatus::ACCEPTED;
    case INIT:
      return roq::OrderStatus::ACCEPTED;
    case UNTRIGGERED:
      return roq::OrderStatus::UNDEFINED;
    case TRIGGERED:
      return roq::OrderStatus::UNDEFINED;
    case DEACTIVATED:
      return roq::OrderStatus::SUSPENDED;
    case REJECTED:
      return roq::OrderStatus::REJECTED;
    case NEW:
      return roq::OrderStatus::WORKING;
    case PARTIALLY_FILLED:
      return roq::OrderStatus::WORKING;
    case FILLED:
      return roq::OrderStatus::COMPLETED;
    case CANCELED:
      return roq::OrderStatus::CANCELED;
  }
  return {};
}

static_assert(Helper{phemex_futures::json::OrderStatus{phemex_futures::json::OrderStatus::UNDEFINED_INTERNAL}} == roq::OrderStatus::UNDEFINED);
static_assert(Helper{phemex_futures::json::OrderStatus{phemex_futures::json::OrderStatus::CREATED}} == roq::OrderStatus::ACCEPTED);
static_assert(Helper{phemex_futures::json::OrderStatus{phemex_futures::json::OrderStatus::INIT}} == roq::OrderStatus::ACCEPTED);
static_assert(Helper{phemex_futures::json::OrderStatus{phemex_futures::json::OrderStatus::UNTRIGGERED}} == roq::OrderStatus::UNDEFINED);
static_assert(Helper{phemex_futures::json::OrderStatus{phemex_futures::json::OrderStatus::TRIGGERED}} == roq::OrderStatus::UNDEFINED);
static_assert(Helper{phemex_futures::json::OrderStatus{phemex_futures::json::OrderStatus::DEACTIVATED}} == roq::OrderStatus::SUSPENDED);
static_assert(Helper{phemex_futures::json::OrderStatus{phemex_futures::json::OrderStatus::REJECTED}} == roq::OrderStatus::REJECTED);
static_assert(Helper{phemex_futures::json::OrderStatus{phemex_futures::json::OrderStatus::NEW}} == roq::OrderStatus::WORKING);
static_assert(Helper{phemex_futures::json::OrderStatus{phemex_futures::json::OrderStatus::PARTIALLY_FILLED}} == roq::OrderStatus::WORKING);
static_assert(Helper{phemex_futures::json::OrderStatus{phemex_futures::json::OrderStatus::FILLED}} == roq::OrderStatus::COMPLETED);
static_assert(Helper{phemex_futures::json::OrderStatus{phemex_futures::json::OrderStatus::CANCELED}} == roq::OrderStatus::CANCELED);

template <>
template <>
std::optional<roq::OrderStatus> Map<phemex_futures::json::OrderStatus>::helper() const {
  return Helper{args_};
}

// phemex_futures::json::OrderType => roq::OrderType

template <>
template <>
constexpr Helper<phemex_futures::json::OrderType>::operator std::optional<roq::OrderType>() const {
  switch (std::get<0>(args_)) {
    using enum phemex_futures::json::OrderType::type_t;
    case UNDEFINED_INTERNAL:
      return roq::OrderType::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::OrderType::UNDEFINED;
    case LIMIT:
      return roq::OrderType::LIMIT;
    case MARKET:
      return roq::OrderType::MARKET;
    case STOP:
      return roq::OrderType::MARKET;
    case STOP_LIMIT:
      return roq::OrderType::LIMIT;
    case MARKET_IF_TOUCHED:
      return roq::OrderType::MARKET;
    case LIMIT_IF_TOUCHED:
      return roq::OrderType::LIMIT;
  }
  return {};
}

static_assert(Helper{phemex_futures::json::OrderType{phemex_futures::json::OrderType::UNDEFINED_INTERNAL}} == roq::OrderType::UNDEFINED);
static_assert(Helper{phemex_futures::json::OrderType{phemex_futures::json::OrderType::LIMIT}} == roq::OrderType::LIMIT);
static_assert(Helper{phemex_futures::json::OrderType{phemex_futures::json::OrderType::MARKET}} == roq::OrderType::MARKET);
static_assert(Helper{phemex_futures::json::OrderType{phemex_futures::json::OrderType::STOP}} == roq::OrderType::MARKET);
static_assert(Helper{phemex_futures::json::OrderType{phemex_futures::json::OrderType::STOP_LIMIT}} == roq::OrderType::LIMIT);
static_assert(Helper{phemex_futures::json::OrderType{phemex_futures::json::OrderType::MARKET_IF_TOUCHED}} == roq::OrderType::MARKET);
static_assert(Helper{phemex_futures::json::OrderType{phemex_futures::json::OrderType::LIMIT_IF_TOUCHED}} == roq::OrderType::LIMIT);

template <>
template <>
std::optional<roq::OrderType> Map<phemex_futures::json::OrderType>::helper() const {
  return Helper{args_};
}

// {phemex_futures::json::PosSide, phemex_futures::json::Side} => roq::PositionEffect

template <>
template <>
constexpr Helper<phemex_futures::json::PosSide, phemex_futures::json::Side>::operator std::optional<roq::PositionEffect>() const {
  switch (std::get<0>(args_)) {
    using enum phemex_futures::json::PosSide::type_t;
    case UNDEFINED_INTERNAL:
      return roq::PositionEffect::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::PositionEffect::UNDEFINED;
    case NONE:
      return roq::PositionEffect::UNDEFINED;
    case LONG:
      switch (std::get<1>(args_)) {
        using enum phemex_futures::json::Side::type_t;
        case UNDEFINED_INTERNAL:
          return roq::PositionEffect::UNDEFINED;
        case UNKNOWN_INTERNAL:
          return roq::PositionEffect::UNDEFINED;
        case NONE:
          return roq::PositionEffect::UNDEFINED;
        case BUY:
          return roq::PositionEffect::OPEN;
        case SELL:
          return roq::PositionEffect::CLOSE;
      }
      break;
    case SHORT:
      switch (std::get<1>(args_)) {
        using enum phemex_futures::json::Side::type_t;
        case UNDEFINED_INTERNAL:
          return roq::PositionEffect::UNDEFINED;
        case UNKNOWN_INTERNAL:
          return roq::PositionEffect::UNDEFINED;
        case NONE:
          return roq::PositionEffect::UNDEFINED;
        case BUY:
          return roq::PositionEffect::CLOSE;
        case SELL:
          return roq::PositionEffect::OPEN;
      }
      break;
    case MERGED:
      return roq::PositionEffect::UNDEFINED;
  }
  return {};
}

static_assert(
    Helper{
        phemex_futures::json::PosSide{phemex_futures::json::PosSide::UNDEFINED_INTERNAL},
        phemex_futures::json::Side{phemex_futures::json::Side::UNDEFINED_INTERNAL}} == roq::PositionEffect::UNDEFINED);
static_assert(
    Helper{phemex_futures::json::PosSide{phemex_futures::json::PosSide::NONE}, phemex_futures::json::Side{phemex_futures::json::Side::BUY}} ==
    roq::PositionEffect::UNDEFINED);
static_assert(
    Helper{phemex_futures::json::PosSide{phemex_futures::json::PosSide::NONE}, phemex_futures::json::Side{phemex_futures::json::Side::SELL}} ==
    roq::PositionEffect::UNDEFINED);
static_assert(
    Helper{phemex_futures::json::PosSide{phemex_futures::json::PosSide::LONG}, phemex_futures::json::Side{phemex_futures::json::Side::BUY}} ==
    roq::PositionEffect::OPEN);
static_assert(
    Helper{phemex_futures::json::PosSide{phemex_futures::json::PosSide::LONG}, phemex_futures::json::Side{phemex_futures::json::Side::SELL}} ==
    roq::PositionEffect::CLOSE);
static_assert(
    Helper{phemex_futures::json::PosSide{phemex_futures::json::PosSide::SHORT}, phemex_futures::json::Side{phemex_futures::json::Side::BUY}} ==
    roq::PositionEffect::CLOSE);
static_assert(
    Helper{phemex_futures::json::PosSide{phemex_futures::json::PosSide::SHORT}, phemex_futures::json::Side{phemex_futures::json::Side::SELL}} ==
    roq::PositionEffect::OPEN);
static_assert(
    Helper{phemex_futures::json::PosSide{phemex_futures::json::PosSide::MERGED}, phemex_futures::json::Side{phemex_futures::json::Side::BUY}} ==
    roq::PositionEffect::UNDEFINED);
static_assert(
    Helper{phemex_futures::json::PosSide{phemex_futures::json::PosSide::MERGED}, phemex_futures::json::Side{phemex_futures::json::Side::SELL}} ==
    roq::PositionEffect::UNDEFINED);

template <>
template <>
std::optional<roq::PositionEffect> Map<phemex_futures::json::PosSide, phemex_futures::json::Side>::helper() const {
  return Helper{args_};
}

// phemex_futures::json::Side => roq::Side

template <>
template <>
constexpr Helper<phemex_futures::json::Side>::operator std::optional<roq::Side>() const {
  switch (std::get<0>(args_)) {
    using enum phemex_futures::json::Side::type_t;
    case UNDEFINED_INTERNAL:
      return roq::Side::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::Side::UNDEFINED;
    case NONE:
      return roq::Side::UNDEFINED;
    case BUY:
      return roq::Side::BUY;
    case SELL:
      return roq::Side::SELL;
  }
  return {};
}

static_assert(Helper{phemex_futures::json::Side{phemex_futures::json::Side::UNDEFINED_INTERNAL}} == roq::Side::UNDEFINED);
static_assert(Helper{phemex_futures::json::Side{phemex_futures::json::Side::NONE}} == roq::Side::UNDEFINED);
static_assert(Helper{phemex_futures::json::Side{phemex_futures::json::Side::BUY}} == roq::Side::BUY);
static_assert(Helper{phemex_futures::json::Side{phemex_futures::json::Side::SELL}} == roq::Side::SELL);

template <>
template <>
std::optional<roq::Side> Map<phemex_futures::json::Side>::helper() const {
  return Helper{args_};
}

// phemex_futures::json::TimeInForce => roq::TimeInForce

template <>
template <>
constexpr Helper<phemex_futures::json::TimeInForce>::operator std::optional<roq::TimeInForce>() const {
  switch (std::get<0>(args_)) {
    using enum phemex_futures::json::TimeInForce::type_t;
    case UNDEFINED_INTERNAL:
      return roq::TimeInForce::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::TimeInForce::UNDEFINED;
    case IMMEDIATE_OR_CANCEL:
      return roq::TimeInForce::IOC;
    case FILL_OR_KILL:
      return roq::TimeInForce::FOK;
    case POST_ONLY:
      return roq::TimeInForce::GTC;
    case RPI_POST_ONLY:
      return roq::TimeInForce::GTC;  // ???
    case GOOD_TILL_CANCEL:
      return roq::TimeInForce::GTC;  // ???
    case GOOD_TILL_MODIFY:
      return roq::TimeInForce::GTC;  // ???
  }
  return {};
}

static_assert(Helper{phemex_futures::json::TimeInForce{phemex_futures::json::TimeInForce::UNDEFINED_INTERNAL}} == roq::TimeInForce::UNDEFINED);
static_assert(Helper{phemex_futures::json::TimeInForce{phemex_futures::json::TimeInForce::IMMEDIATE_OR_CANCEL}} == roq::TimeInForce::IOC);
static_assert(Helper{phemex_futures::json::TimeInForce{phemex_futures::json::TimeInForce::FILL_OR_KILL}} == roq::TimeInForce::FOK);
static_assert(Helper{phemex_futures::json::TimeInForce{phemex_futures::json::TimeInForce::POST_ONLY}} == roq::TimeInForce::GTC);
static_assert(Helper{phemex_futures::json::TimeInForce{phemex_futures::json::TimeInForce::RPI_POST_ONLY}} == roq::TimeInForce::GTC);
static_assert(Helper{phemex_futures::json::TimeInForce{phemex_futures::json::TimeInForce::GOOD_TILL_CANCEL}} == roq::TimeInForce::GTC);
static_assert(Helper{phemex_futures::json::TimeInForce{phemex_futures::json::TimeInForce::GOOD_TILL_MODIFY}} == roq::TimeInForce::GTC);

template <>
template <>
std::optional<roq::TimeInForce> Map<phemex_futures::json::TimeInForce>::helper() const {
  return Helper{args_};
}

// phemex_futures::json::Type => roq::SecurityType

template <>
template <>
constexpr Helper<phemex_futures::json::Type>::operator std::optional<roq::SecurityType>() const {
  switch (std::get<0>(args_)) {
    using enum phemex_futures::json::Type::type_t;
    case UNDEFINED_INTERNAL:
      return roq::SecurityType::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::SecurityType::UNDEFINED;
    case SPOT:
      return roq::SecurityType::SPOT;
    case PERPETUAL:
      return roq::SecurityType::SWAP;
    case PERPETUAL_V2:
      return roq::SecurityType::SWAP;
  }
  return {};
}

static_assert(Helper{phemex_futures::json::Type{phemex_futures::json::Type::UNDEFINED_INTERNAL}} == roq::SecurityType::UNDEFINED);
static_assert(Helper{phemex_futures::json::Type{phemex_futures::json::Type::SPOT}} == roq::SecurityType::SPOT);
static_assert(Helper{phemex_futures::json::Type{phemex_futures::json::Type::PERPETUAL}} == roq::SecurityType::SWAP);
static_assert(Helper{phemex_futures::json::Type{phemex_futures::json::Type::PERPETUAL_V2}} == roq::SecurityType::SWAP);

template <>
template <>
std::optional<roq::SecurityType> Map<phemex_futures::json::Type>::helper() const {
  return Helper{args_};
}

// roq => phemex_futures::json

// roq::OrderType => phemex_futures::json::OrderType

template <>
template <>
constexpr Helper<roq::OrderType>::operator std::optional<phemex_futures::json::OrderType>() const {
  switch (std::get<0>(args_)) {
    using enum roq::OrderType;
    case UNDEFINED:
      return phemex_futures::json::OrderType::UNDEFINED_INTERNAL;
    case MARKET:
      return phemex_futures::json::OrderType::MARKET;
    case LIMIT:
      return phemex_futures::json::OrderType::LIMIT;
  }
  return {};
}

static_assert(Helper{roq::OrderType::UNDEFINED} == phemex_futures::json::OrderType{phemex_futures::json::OrderType::UNDEFINED_INTERNAL});
static_assert(Helper{roq::OrderType::MARKET} == phemex_futures::json::OrderType{phemex_futures::json::OrderType::MARKET});
static_assert(Helper{roq::OrderType::LIMIT} == phemex_futures::json::OrderType{phemex_futures::json::OrderType::LIMIT});

template <>
template <>
std::optional<phemex_futures::json::OrderType> Map<roq::OrderType>::helper() const {
  return Helper{args_};
}

// {roq::PositionEffect, roq::Side} => phemex_futures::json::PosSide

template <>
template <>
constexpr Helper<roq::PositionEffect, roq::Side>::operator std::optional<phemex_futures::json::PosSide>() const {
  switch (std::get<0>(args_)) {
    using enum roq::PositionEffect;
    case UNDEFINED:
      return phemex_futures::json::PosSide::MERGED;  // note!
    case OPEN:
      switch (std::get<1>(args_)) {
        using enum roq::Side;
        case UNDEFINED:
          return phemex_futures::json::PosSide::UNDEFINED_INTERNAL;
        case BUY:
          return phemex_futures::json::PosSide::LONG;
        case SELL:
          return phemex_futures::json::PosSide::SHORT;
      }
      break;
    case CLOSE:
      switch (std::get<1>(args_)) {
        using enum roq::Side;
        case UNDEFINED:
          return phemex_futures::json::PosSide::UNDEFINED_INTERNAL;
        case BUY:
          return phemex_futures::json::PosSide::SHORT;
        case SELL:
          return phemex_futures::json::PosSide::LONG;
      }
      break;
  }
  return {};
}

static_assert(Helper{roq::PositionEffect::UNDEFINED, roq::Side::UNDEFINED} == phemex_futures::json::PosSide{phemex_futures::json::PosSide::MERGED});
static_assert(Helper{roq::PositionEffect::OPEN, roq::Side::BUY} == phemex_futures::json::PosSide{phemex_futures::json::PosSide::LONG});
static_assert(Helper{roq::PositionEffect::OPEN, roq::Side::SELL} == phemex_futures::json::PosSide{phemex_futures::json::PosSide::SHORT});
static_assert(Helper{roq::PositionEffect::CLOSE, roq::Side::BUY} == phemex_futures::json::PosSide{phemex_futures::json::PosSide::SHORT});
static_assert(Helper{roq::PositionEffect::CLOSE, roq::Side::SELL} == phemex_futures::json::PosSide{phemex_futures::json::PosSide::LONG});

template <>
template <>
std::optional<phemex_futures::json::PosSide> Map<roq::PositionEffect, roq::Side>::helper() const {
  return Helper{args_};
}

// roq::Side => phemex_futures::json::Side

template <>
template <>
constexpr Helper<roq::Side>::operator std::optional<phemex_futures::json::Side>() const {
  switch (std::get<0>(args_)) {
    using enum roq::Side;
    case UNDEFINED:
      return phemex_futures::json::Side::UNDEFINED_INTERNAL;
    case BUY:
      return phemex_futures::json::Side::BUY;
    case SELL:
      return phemex_futures::json::Side::SELL;
  }
  return {};
}

static_assert(Helper{roq::Side::UNDEFINED} == phemex_futures::json::Side{phemex_futures::json::Side::UNDEFINED_INTERNAL});
static_assert(Helper{roq::Side::BUY} == phemex_futures::json::Side{phemex_futures::json::Side::BUY});
static_assert(Helper{roq::Side::SELL} == phemex_futures::json::Side{phemex_futures::json::Side::SELL});

template <>
template <>
std::optional<phemex_futures::json::Side> Map<roq::Side>::helper() const {
  return Helper{args_};
}

// roq::TimeInForce => phemex_futures::json::Force

// POST_ONLY ???
template <>
template <>
constexpr Helper<roq::TimeInForce>::operator std::optional<phemex_futures::json::TimeInForce>() const {
  switch (std::get<0>(args_)) {
    using enum roq::TimeInForce;
    case UNDEFINED:
      return phemex_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case GFD:
      return phemex_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case GTC:
      return phemex_futures::json::TimeInForce::GOOD_TILL_CANCEL;
    case OPG:
      return phemex_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case IOC:
      return phemex_futures::json::TimeInForce::IMMEDIATE_OR_CANCEL;
    case FOK:
      return phemex_futures::json::TimeInForce::FILL_OR_KILL;
    case GTX:
      return phemex_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case GTD:
      return phemex_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case AT_THE_CLOSE:
      return phemex_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case GOOD_THROUGH_CROSSING:
      return phemex_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case AT_CROSSING:
      return phemex_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case GOOD_FOR_TIME:
      return phemex_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case GFA:
      return phemex_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case GFM:
      return phemex_futures::json::TimeInForce::UNDEFINED_INTERNAL;
  }
  return {};
}

static_assert(Helper{roq::TimeInForce::UNDEFINED} == phemex_futures::json::TimeInForce{phemex_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GFD} == phemex_futures::json::TimeInForce{phemex_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GTC} == phemex_futures::json::TimeInForce{phemex_futures::json::TimeInForce::GOOD_TILL_CANCEL});
static_assert(Helper{roq::TimeInForce::OPG} == phemex_futures::json::TimeInForce{phemex_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::IOC} == phemex_futures::json::TimeInForce{phemex_futures::json::TimeInForce::IMMEDIATE_OR_CANCEL});
static_assert(Helper{roq::TimeInForce::FOK} == phemex_futures::json::TimeInForce{phemex_futures::json::TimeInForce::FILL_OR_KILL});
static_assert(Helper{roq::TimeInForce::GTX} == phemex_futures::json::TimeInForce{phemex_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GTD} == phemex_futures::json::TimeInForce{phemex_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::AT_THE_CLOSE} == phemex_futures::json::TimeInForce{phemex_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GOOD_THROUGH_CROSSING} == phemex_futures::json::TimeInForce{phemex_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::AT_CROSSING} == phemex_futures::json::TimeInForce{phemex_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GOOD_FOR_TIME} == phemex_futures::json::TimeInForce{phemex_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GFA} == phemex_futures::json::TimeInForce{phemex_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GFM} == phemex_futures::json::TimeInForce{phemex_futures::json::TimeInForce::UNDEFINED_INTERNAL});

template <>
template <>
std::optional<phemex_futures::json::TimeInForce> Map<roq::TimeInForce>::helper() const {
  return Helper{args_};
}

}  // namespace roq
