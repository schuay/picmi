set(PICMI_VERSION_MAJOR 2)
set(PICMI_VERSION_MINOR 0)
set(PICMI_VERSION_PATCH 0)
if (NOT PICMI_FILEPATH)
    set(PICMI_FILEPATH ${KDE_INSTALL_DATADIR}/picmi/)
endif ()

add_definitions(${QT_DEFINITIONS} ${KDE_DEFINITIONS})
remove_definitions(-DQT_NO_STL)

kde_enable_exceptions()

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions("-Wextra -pedantic")
endif ()

include(CheckIncludeFileCXX)
add_definitions(${QT_DEFINITIONS})

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

include_directories(
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

file(GLOB ICONS_SRCS "${CMAKE_SOURCE_DIR}/icons/*-apps-picmi.png")
ecm_add_app_icon(picmi_SRCS ICONS ${ICONS_SRCS})

add_executable(picmi ${picmi_SRCS})
target_link_libraries(picmi
    picmi_gui
    picmi_logic
    KF5::CoreAddons
    KF5::XmlGui
    KF5::I18n
    Qt5::Svg
    KF5KDEGames
    KF5::DBusAddons
    KF5::KDELibs4Support
)

install(TARGETS picmi DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

# vim:set ts=4 sw=4 et:
