set(TARGET_NAME ${PROJECT_NAME}-protocol-json)

set(SOURCES encoder.cpp map.cpp parser.cpp parser_2.cpp utils.cpp)

include(RoqAutogen)

set(AUTOGEN_SCHEMAS
    accounts_orders_positions.json
    accounts_orders_positions2.json
    accounts_orders_positions2_accounts_item.json
    accounts_orders_positions2_orders_item.json
    accounts_orders_positions2_positions_item.json
    accounts_orders_positions_accounts_item.json
    accounts_orders_positions_orders_item.json
    accounts_orders_positions_positions_item.json
    ack.json
    ack_error.json
    ack_result.json
    ack_result_status.json
    action.json
    book.json
    book_data.json
    book_data_item.json
    event.json
    event_type.json
    exec_inst.json
    exec_status.json
    header.json
    index_market24h.json
    index_market24h_data.json
    kline.json
    kline2.json
    kline2_data_item.json
    kline_data_item.json
    liq_status.json
    liquidity_ind.json
    margin_mode.json
    market24h.json
    market24h2.json
    market24h2_data.json
    market24h_data.json
    message_field.json
    message_type.json
    order_status.json
    order_type.json
    orderbook.json
    orderbook_data.json
    orderbook_data_item.json
    orders_all_ack.json
    orders_all_ack2.json
    orders_cancel_ack.json
    orders_cancel_ack2.json
    orders_cancel_ack2_data.json
    orders_cancel_ack_data.json
    orders_create_ack.json
    orders_create_ack2.json
    orders_create_ack2_data.json
    orders_create_ack_data.json
    orders_replace_ack.json
    orders_replace_ack2.json
    orders_replace_ack2_data.json
    orders_replace_ack_data.json
    pong.json
    pos_mode.json
    pos_side.json
    position_info.json
    position_info_position_info.json
    products_ack.json
    products_ack_data.json
    products_ack_data_currencies_item.json
    products_ack_data_perp_products_v2_item.json
    products_ack_data_products_item.json
    side.json
    status.json
    stp_instruction.json
    time_in_force.json
    trade_type.json
    trades.json
    trades2.json
    trades2_trades_item.json
    trades_trades_item.json
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

add_custom_target(protocol-json-autogen-headers ALL DEPENDS ${AUTOGEN_HEADERS})

roq_autogen_cpp(
  OUTPUT
  AUTOGEN_SOURCES
  SOURCES
  ${AUTOGEN_SCHEMAS}
  TEMPLATE_DIR
  ${TEMPLATE_DIR}
  TEMPLATE_TYPE
  "json")

roq_gitignore(OUTPUT .gitignore SOURCES ${TARGET_NAME} ${AUTOGEN_HEADERS} ${AUTOGEN_SOURCES})

add_library(${TARGET_NAME} OBJECT ${SOURCES} ${AUTOGEN_SOURCES})

add_dependencies(${TARGET_NAME} protocol-json-autogen-headers)

target_link_libraries(${TARGET_NAME} PRIVATE roq-core::roq-core roq-codec::roq-codec roq-utils::roq-utils roq-logging::roq-logging fmt::fmt)
