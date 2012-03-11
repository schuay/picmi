set(PICMI_VERSION_MAJOR 1)
set(PICMI_VERSION_MINOR 9)
set(PICMI_VERSION_PATCH 95)
if (NOT PICMI_FILEPATH)
    set(PICMI_FILEPATH ${DATA_INSTALL_DIR}/picmi/)
endif ()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

add_definitions("-std=c++0x")
add_definitions(${KDE4_ENABLE_EXCEPTIONS})

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

include_directories(
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

install(TARGETS picmi DESTINATION bin)

# vim:set ts=4 sw=4 et:
