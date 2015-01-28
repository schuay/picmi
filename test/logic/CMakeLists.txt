set(streaks_test_SRCS
    streaks_test.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/src/logic
    ${CMAKE_SOURCE_DIR}
)

add_executable(streaks_test ${streaks_test_SRCS})
add_test(streaks_test streaks_test)
ecm_mark_as_test(streaks_test)

target_link_libraries(streaks_test picmi_logic Qt5::Test ${QT_LIBRARIES} )

# vim:set ts=4 sw=4 et:
