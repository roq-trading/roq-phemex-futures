include(RoqAutogen)

set(TARGET_NAME ${PROJECT_NAME}-test)

set(SOURCES
    json_accounts_orders_positions_2.cpp
    json_accounts_orders_positions.cpp
    json_book.cpp
    json_index_market24h.cpp
    json_kline_2.cpp
    json_kline.cpp
    json_market24h_2.cpp
    json_market24h.cpp
    json_orderbook.cpp
    json_orders_cancel_ack_2.cpp
    json_orders_cancel_ack.cpp
    json_orders_create_ack_2.cpp
    json_orders_create_ack.cpp
    json_orders_replace_ack_2.cpp
    json_orders_replace_ack.cpp
    json_position_info.cpp
    json_products_ack.cpp
    json_trades_2.cpp
    json_trades.cpp
    main.cpp)

roq_gitignore(OUTPUT .gitignore SOURCES ${TARGET_NAME})

add_executable(${TARGET_NAME} ${SOURCES})

target_link_libraries(${TARGET_NAME} ${PROJECT_NAME}-json roq-server::roq-server Catch2::Catch2)

if(ROQ_BUILD_TYPE STREQUAL "Release")
  set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS_RELEASE -s)
endif()

add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
