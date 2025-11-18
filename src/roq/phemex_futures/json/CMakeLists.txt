set(TARGET_NAME "${PROJECT_NAME}-json")

set(SOURCES encoder.cpp map.cpp parser.cpp utils.cpp)

include(RoqAutogen)

set(AUTOGEN_SCHEMAS
    account_assets_data_assets_item.json
    account_assets_data.json
    account_assets.json
    account_positions_data_account.json
    account_positions_data.json
    account_positions_data_positions_item.json
    account_positions.json
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
    arg.json # XXX
    book_data_item.json
    book_data.json
    book.json
    cancel_all_orders_ack.json
    cancel_order_ack_data.json
    cancel_order_ack.json
    category.json # XXX
    event.json
    event_type.json
    exec_inst.json
    exec_status.json
    fill_data_item.json
    fill_history_data.json
    fill_history_data_list_item_fee_detail_item.json
    fill_history_data_list_item.json
    fill_history.json
    fill.json
    hold_side.json # XXX
    index_market24h_data.json
    index_market24h.json
    kline_data_item.json
    kline.json
    liq_status.json
    market24h2.json
    market24h_data2.json
    market24h_data.json
    market24h.json
    message_field.json
    message_type.json
    modify_order_ack_data.json
    modify_order_ack.json
    open_orders_data.json
    open_orders_data_rows_item.json
    open_orders.json
    order_status.json
    order_type.json
    place_order_ack_data.json
    place_order_ack.json
    pong.json
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
    topic.json # XXX
    trade_scope.json # XXX
    trade_side.json # XXX
    trades.json
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
