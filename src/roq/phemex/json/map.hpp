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

#include "roq/phemex/json/action.hpp"
#include "roq/phemex/json/asset_mode.hpp"
#include "roq/phemex/json/category.hpp"
#include "roq/phemex/json/futures_type.hpp"
#include "roq/phemex/json/margin_mode.hpp"
#include "roq/phemex/json/message_type.hpp"
#include "roq/phemex/json/order_status.hpp"
#include "roq/phemex/json/order_type.hpp"
#include "roq/phemex/json/pos_side.hpp"
#include "roq/phemex/json/side.hpp"
#include "roq/phemex/json/time_in_force.hpp"
#include "roq/phemex/json/trade_scope.hpp"
#include "roq/phemex/json/trade_side.hpp"
#include "roq/phemex/json/trading_status.hpp"

namespace roq {

// phemex::json => roq

template <>
template <>
std::optional<UpdateType> Map<phemex::json::Action>::helper() const;

template <>
template <>
std::optional<MarginMode> Map<phemex::json::AssetMode>::helper() const;

template <>
template <>
std::optional<SecurityType> Map<phemex::json::Category, phemex::json::FuturesType>::helper() const;

template <>
template <>
std::optional<MarginMode> Map<phemex::json::MarginMode>::helper() const;

template <>
template <>
std::optional<UpdateType> Map<phemex::json::MessageType>::helper() const;

template <>
template <>
std::optional<OrderStatus> Map<phemex::json::OrderStatus>::helper() const;

template <>
template <>
std::optional<OrderType> Map<phemex::json::OrderType>::helper() const;

template <>
template <>
std::optional<PositionEffect> Map<phemex::json::PosSide, phemex::json::Side>::helper() const;

template <>
template <>
std::optional<Side> Map<phemex::json::Side>::helper() const;

template <>
template <>
std::optional<TimeInForce> Map<phemex::json::TimeInForce>::helper() const;

template <>
template <>
std::optional<Liquidity> Map<phemex::json::TradeScope>::helper() const;

template <>
template <>
std::optional<PositionEffect> Map<phemex::json::TradeSide>::helper() const;

template <>
template <>
std::optional<TradingStatus> Map<phemex::json::TradingStatus>::helper() const;

// roq => phemex::json

template <>
template <>
std::optional<phemex::json::MarginMode> Map<MarginMode>::helper() const;

template <>
template <>
std::optional<phemex::json::OrderType> Map<OrderType>::helper() const;

template <>
template <>
std::optional<phemex::json::PosSide> Map<PositionEffect, Side>::helper() const;

template <>
template <>
std::optional<phemex::json::TradeSide> Map<PositionEffect>::helper() const;

template <>
template <>
std::optional<phemex::json::Side> Map<Side>::helper() const;

template <>
template <>
std::optional<phemex::json::TimeInForce> Map<TimeInForce>::helper() const;

}  // namespace roq
