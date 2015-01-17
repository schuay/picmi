cmake_minimum_required (VERSION 2.8.12)
project (picmi)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 5.2.0 CONFIG REQUIRED Core Widgets Svg Quick QuickWidgets Test)
find_package(KF5 REQUIRED COMPONENTS
    CoreAddons
    Declarative
    I18n
    KDELibs4Support
    NewStuff
    XmlGui
)

find_package(KF5KDEGames 5.2.0 REQUIRED)

include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)
include(FeatureSummary)
include(ECMInstallIcons)

add_subdirectory(themes)
add_subdirectory(icons)
add_subdirectory(doc)
add_subdirectory(levels)
add_subdirectory(src)
add_subdirectory(test)

install(FILES ${CMAKE_SOURCE_DIR}/org.kde.picmi.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES ${CMAKE_SOURCE_DIR}/picmi.appdata.xml DESTINATION ${SHARE_INSTALL_PREFIX}/appdata )
install(FILES ${CMAKE_SOURCE_DIR}/picmiui.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/picmi)

# vim:set ts=4 sw=4 et:
