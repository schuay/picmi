set(streaks_test_SRCS
    streaks_test.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/src/logic
    ${CMAKE_SOURCE_DIR}
)

kde4_add_unit_test(streaks_test
    TESTNAME streaks-test
    ${streaks_test_SRCS}
)

target_link_libraries(streaks_test
    picmi_logic
    ${QT_QTTEST_LIBRARY}
    ${QT_LIBRARIES}
)

# vim:set ts=4 sw=4 et:
