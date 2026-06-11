/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include "roq/map.hpp"

#include "roq/liquidity.hpp"
#include "roq/order_status.hpp"
#include "roq/order_type.hpp"
#include "roq/position_effect.hpp"
#include "roq/security_type.hpp"
#include "roq/side.hpp"
#include "roq/time_in_force.hpp"
#include "roq/update_type.hpp"

#include "roq/phemex_futures/protocol/json/event_type.hpp"
#include "roq/phemex_futures/protocol/json/liquidity_ind.hpp"
#include "roq/phemex_futures/protocol/json/message_type.hpp"
#include "roq/phemex_futures/protocol/json/order_status.hpp"
#include "roq/phemex_futures/protocol/json/order_type.hpp"
#include "roq/phemex_futures/protocol/json/pos_side.hpp"
#include "roq/phemex_futures/protocol/json/side.hpp"
#include "roq/phemex_futures/protocol/json/time_in_force.hpp"
#include "roq/phemex_futures/protocol/json/type.hpp"

namespace roq {

// phemex_futures::json => roq

template <>
template <>
std::optional<UpdateType> Map<phemex_futures::protocol::json::EventType>::helper() const;

template <>
template <>
std::optional<Liquidity> Map<phemex_futures::protocol::json::LiquidityInd>::helper() const;

template <>
template <>
std::optional<UpdateType> Map<phemex_futures::protocol::json::MessageType>::helper() const;

template <>
template <>
std::optional<OrderStatus> Map<phemex_futures::protocol::json::OrderStatus>::helper() const;

template <>
template <>
std::optional<OrderType> Map<phemex_futures::protocol::json::OrderType>::helper() const;

template <>
template <>
std::optional<PositionEffect> Map<phemex_futures::protocol::json::PosSide, phemex_futures::protocol::json::Side>::helper() const;

template <>
template <>
std::optional<Side> Map<phemex_futures::protocol::json::Side>::helper() const;

template <>
template <>
std::optional<TimeInForce> Map<phemex_futures::protocol::json::TimeInForce>::helper() const;

template <>
template <>
std::optional<SecurityType> Map<phemex_futures::protocol::json::Type>::helper() const;

// roq => phemex_futures::json

template <>
template <>
std::optional<phemex_futures::protocol::json::OrderType> Map<OrderType>::helper() const;

template <>
template <>
std::optional<phemex_futures::protocol::json::PosSide> Map<PositionEffect, Side>::helper() const;

template <>
template <>
std::optional<phemex_futures::protocol::json::Side> Map<Side>::helper() const;

template <>
template <>
std::optional<phemex_futures::protocol::json::TimeInForce> Map<TimeInForce>::helper() const;

}  // namespace roq
