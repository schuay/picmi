set(logic_SRCS
    board.cpp
    boardmap.cpp
    boardstate.cpp
    elapsedtime.cpp
    levelloader.cpp
    picmi.cpp
    streaks.cpp
)

add_library(picmi_logic STATIC
    ${logic_SRCS}
)

target_link_libraries(picmi_logic
    ${KDEGAMES_LIBRARY}
    ${KDE4_KDECORE_LIBS}
    ${QT_LIBRARIES}
)

# vim:set ts=4 sw=4 et:
