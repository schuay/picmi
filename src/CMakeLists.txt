set(PICMI_VERSION_MAJOR 2)
set(PICMI_VERSION_MINOR 0)
set(PICMI_VERSION_PATCH 0)
if (NOT PICMI_FILEPATH)
    set(PICMI_FILEPATH ${DATA_INSTALL_DIR}/picmi/)
endif ()

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
remove_definitions(-DQT_NO_STL)

# See http://www.cmake.org/pipermail/cmake/2008-June/022381.html
# and 2a38192c6daba in KDE SVN.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions("-Wextra -pedantic")
endif ()

include(CheckIncludeFileCXX)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

include_directories(
    ${KDE4_INCLUDES}
    ${KDEGAMES_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_subdirectory(gui)
add_subdirectory(logic)

set(picmi_SRCS
    main.cpp
    outofboundsexception.cpp
    settings.cpp
    systemexception.cpp
)

kde4_add_app_icon(picmi_SRCS "${CMAKE_SOURCE_DIR}/icons/hi*-apps-picmi.png")
kde4_add_executable(picmi
    ${picmi_SRCS}
)

set(picmi_LIBS picmi_gui picmi_logic)

target_link_libraries(picmi
    ${picmi_LIBS}
    ${KDEGAMES_LIBRARY}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KDECORE_LIBS}
    ${QT_LIBRARIES}
)

install(TARGETS picmi DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})

# vim:set ts=4 sw=4 et:
