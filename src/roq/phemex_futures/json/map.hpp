/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include "roq/map.hpp"

#include "roq/liquidity.hpp"
#include "roq/margin_mode.hpp"
#include "roq/order_status.hpp"
#include "roq/order_type.hpp"
#include "roq/position_effect.hpp"
#include "roq/security_type.hpp"
#include "roq/side.hpp"
#include "roq/time_in_force.hpp"
#include "roq/trading_status.hpp"
#include "roq/update_type.hpp"

#include "roq/phemex_futures/json/action.hpp"
#include "roq/phemex_futures/json/asset_mode.hpp"
#include "roq/phemex_futures/json/category.hpp"
#include "roq/phemex_futures/json/event_type.hpp"
#include "roq/phemex_futures/json/futures_type.hpp"
#include "roq/phemex_futures/json/margin_mode.hpp"
#include "roq/phemex_futures/json/message_type.hpp"
#include "roq/phemex_futures/json/order_status.hpp"
#include "roq/phemex_futures/json/order_type.hpp"
#include "roq/phemex_futures/json/pos_side.hpp"
#include "roq/phemex_futures/json/side.hpp"
#include "roq/phemex_futures/json/time_in_force.hpp"
#include "roq/phemex_futures/json/trade_scope.hpp"
#include "roq/phemex_futures/json/trade_side.hpp"
#include "roq/phemex_futures/json/trading_status.hpp"
#include "roq/phemex_futures/json/type.hpp"

namespace roq {

// phemex_futures::json => roq

template <>
template <>
std::optional<UpdateType> Map<phemex_futures::json::Action>::helper() const;  // ???

template <>
template <>
std::optional<MarginMode> Map<phemex_futures::json::AssetMode>::helper() const;  // ???

template <>
template <>
std::optional<SecurityType> Map<phemex_futures::json::Category, phemex_futures::json::FuturesType>::helper() const;  // ???

template <>
template <>
std::optional<UpdateType> Map<phemex_futures::json::EventType>::helper() const;

template <>
template <>
std::optional<MarginMode> Map<phemex_futures::json::MarginMode>::helper() const;  // ???

template <>
template <>
std::optional<UpdateType> Map<phemex_futures::json::MessageType>::helper() const;  // ???

template <>
template <>
std::optional<OrderStatus> Map<phemex_futures::json::OrderStatus>::helper() const;

template <>
template <>
std::optional<OrderType> Map<phemex_futures::json::OrderType>::helper() const;

template <>
template <>
std::optional<PositionEffect> Map<phemex_futures::json::PosSide, phemex_futures::json::Side>::helper() const;

template <>
template <>
std::optional<Side> Map<phemex_futures::json::Side>::helper() const;

template <>
template <>
std::optional<TimeInForce> Map<phemex_futures::json::TimeInForce>::helper() const;

template <>
template <>
std::optional<Liquidity> Map<phemex_futures::json::TradeScope>::helper() const;

template <>
template <>
std::optional<PositionEffect> Map<phemex_futures::json::TradeSide>::helper() const;

template <>
template <>
std::optional<TradingStatus> Map<phemex_futures::json::TradingStatus>::helper() const;

template <>
template <>
std::optional<SecurityType> Map<phemex_futures::json::Type>::helper() const;

// roq => phemex_futures::json

template <>
template <>
std::optional<phemex_futures::json::MarginMode> Map<MarginMode>::helper() const;

template <>
template <>
std::optional<phemex_futures::json::OrderType> Map<OrderType>::helper() const;

template <>
template <>
std::optional<phemex_futures::json::PosSide> Map<PositionEffect, Side>::helper() const;

template <>
template <>
std::optional<phemex_futures::json::TradeSide> Map<PositionEffect>::helper() const;

template <>
template <>
std::optional<phemex_futures::json::Side> Map<Side>::helper() const;

template <>
template <>
std::optional<phemex_futures::json::TimeInForce> Map<TimeInForce>::helper() const;

}  // namespace roq
