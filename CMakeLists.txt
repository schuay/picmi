cmake_minimum_required (VERSION 2.8.0)
project (picmi)

if (NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif ()

set(QT_USE_QTSVG TRUE)
set(QT_USE_QTXML TRUE)

find_package(Qt4 REQUIRED)
find_package(KDE4 REQUIRED)
find_package(KDEGames 4.9.0 REQUIRED)

add_subdirectory(themes)
add_subdirectory(icons)
add_subdirectory(doc)
add_subdirectory(levels)
add_subdirectory(src)

install(FILES ${CMAKE_SOURCE_DIR}/picmi.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES ${CMAKE_SOURCE_DIR}/picmiui.rc DESTINATION ${DATA_INSTALL_DIR}/picmi)

# vim:set ts=4 sw=4 et:
