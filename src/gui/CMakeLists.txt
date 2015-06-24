set(gui_SRCS
    mainwindow.cpp
    renderer.cpp
    scene.cpp
    selectboardwindow.cpp
    settingswindow.cpp
    view.cpp
)

set(graphicsitems_SRCS
    graphicsitems/cellitem.cpp
    graphicsitems/highlightitem.cpp
    graphicsitems/pixmapitem.cpp
    graphicsitems/reloadableitem.cpp
    graphicsitems/streakitem.cpp
    graphicsitems/textbanneritem.cpp
)

set(picmi_FORMS
    ${CMAKE_CURRENT_SOURCE_DIR}/settingswindow.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/selectboardwindow.ui
)
ki18n_wrap_ui(picmi_FORMS_HEADERS ${picmi_FORMS})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_library(picmi_gui STATIC
    ${gui_SRCS}
    ${graphicsitems_SRCS}
    ${picmi_FORMS_HEADERS}
)

target_link_libraries(picmi_gui
    KF5KDEGames
    KF5::CoreAddons
    KF5::I18n
    KF5::XmlGui
    Qt5::Core
    Qt5::Svg
)

# vim:set ts=4 sw=4 et:
