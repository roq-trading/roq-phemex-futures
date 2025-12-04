set(TARGET_NAME "${PROJECT_NAME}-json")

set(SOURCES encoder.cpp map.cpp parser.cpp parser_2.cpp utils.cpp)

include(RoqAutogen)

set(AUTOGEN_SCHEMAS
    accounts_orders_positions2_accounts_item.json
    accounts_orders_positions2.json
    accounts_orders_positions2_orders_item.json
    accounts_orders_positions2_positions_item.json
    accounts_orders_positions_accounts_item.json
    accounts_orders_positions.json
    accounts_orders_positions_orders_item.json
    accounts_orders_positions_positions_item.json
    ack_error.json
    ack.json
    ack_result.json
    ack_result_status.json
    action.json
    book_data_item.json
    book_data.json
    book.json
    cancel_all_orders_ack2.json
    cancel_all_orders_ack.json
    cancel_order_ack2_data.json
    cancel_order_ack2.json
    cancel_order_ack_data.json
    cancel_order_ack.json
    event.json
    event_type.json
    exec_inst.json
    exec_status.json
    index_market24h_data.json
    index_market24h.json
    kline2_data_item.json
    kline2.json
    kline_data_item.json
    kline.json
    liq_status.json
    liquidity_ind.json
    margin_mode.json
    market24h2.json
    market24h_data2.json
    market24h_data.json
    market24h.json
    message_field.json
    message_type.json
    modify_order_ack2_data.json
    modify_order_ack2.json
    modify_order_ack_data.json
    modify_order_ack.json
    orderbook.json
    order_status.json
    order_type.json
    place_order_ack2_data.json
    place_order_ack2.json
    place_order_ack_data.json
    place_order_ack.json
    pong.json
    position_info.json
    position_info_position_info.json
    pos_mode.json
    pos_side.json
    products_data_currencies_item.json
    products_data.json
    products_data_perp_products_v2_item.json
    products_data_products_item.json
    products.json
    side.json
    status.json
    stp_instruction.json
    time_in_force.json
    trades2.json
    trades2_trades_item.json
    trades.json
    trades_trades_item.json
    trade_type.json
    type.json
    user_mode.json)

roq_autogen_hpp(
  OUTPUT
  AUTOGEN_HEADERS
  SOURCES
  ${AUTOGEN_SCHEMAS}
  TEMPLATE_DIR
  ${TEMPLATE_DIR}
  TEMPLATE_TYPE
  "json")

add_custom_target(autogen-json-headers ALL DEPENDS ${AUTOGEN_HEADERS})

roq_autogen_cpp(
  OUTPUT
  AUTOGEN_SOURCES
  SOURCES
  ${AUTOGEN_SCHEMAS}
  TEMPLATE_DIR
  ${TEMPLATE_DIR}
  TEMPLATE_TYPE
  "json")

roq_gitignore(OUTPUT .gitignore SOURCES "${TARGET_NAME}" ${AUTOGEN_HEADERS} ${AUTOGEN_SOURCES})

add_library("${TARGET_NAME}" OBJECT ${SOURCES} ${AUTOGEN_SOURCES})

add_dependencies("${TARGET_NAME}" autogen-json-headers)

target_link_libraries("${TARGET_NAME}" PRIVATE roq-core::roq-core roq-codec::roq-codec roq-utils::roq-utils roq-logging::roq-logging fmt::fmt)
