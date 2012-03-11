cmake_minimum_required (VERSION 2.8.0)
project (picmi)

if (NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif ()

set(QT_USE_QTSVG TRUE)
set(QT_USE_QTXML TRUE)

find_package(Qt4 REQUIRED)
find_package(KDE4 REQUIRED)
find_package(LibKDEGames REQUIRED)

set(PICMI_VERSION_MAJOR 1)
set(PICMI_VERSION_MINOR 9)
set(PICMI_VERSION_PATCH 95)
if (NOT PICMI_FILEPATH)
    set(PICMI_FILEPATH ${DATA_INSTALL_DIR}/picmi/)
endif ()

configure_file(
    "${PROJECT_SOURCE_DIR}/src/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
)

add_definitions("-std=c++0x")
add_definitions(${KDE4_ENABLE_EXCEPTIONS})

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

add_subdirectory(themes)
add_subdirectory(icons)
add_subdirectory(doc)
add_subdirectory(levels)

set(picmi_FORMS
    ${CMAKE_SOURCE_DIR}/src/gui/settingswindow.ui
    ${CMAKE_SOURCE_DIR}/src/gui/selectboardwindow.ui
)
qt4_wrap_ui(picmi_FORMS_HEADERS ${picmi_FORMS})


include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${KDEGAMES_INCLUDE_DIR}
)
aux_source_directory(src/ src_sources)
aux_source_directory(src/gui src_gui_sources)
aux_source_directory(src/gui/graphicsitems src_gui_graphicsitems_sources)
aux_source_directory(src/logic src_logic_sources)

kde4_add_executable(picmi
    ${src_sources}
    ${src_gui_sources}
    ${src_gui_graphicsitems_sources}
    ${src_logic_sources}
    ${picmi_FORMS_HEADERS}
)

target_link_libraries(picmi
    ${KDEGAMES_LIBRARY}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KDECORE_LIBS}
    ${QT_LIBRARIES}
)

install(TARGETS picmi DESTINATION bin)
install(FILES ${CMAKE_SOURCE_DIR}/picmi.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES ${CMAKE_SOURCE_DIR}/picmiui.rc DESTINATION ${DATA_INSTALL_DIR}/picmi)

# vim:set ts=4 sw=4 et:
