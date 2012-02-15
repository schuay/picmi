cmake_minimum_required (VERSION 2.8.0)
project (picmi)

if (NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif ()

set(Boost_USE_MULTITHREADED ON)
set(QT_USE_QTSVG TRUE)

set(picmi_VERSION_MAJOR 1)
set(picmi_VERSION_MINOR 9)
set(picmi_VERSION_PATCH 95)
set(picmi_FILEPATH \"${CMAKE_INSTALL_PREFIX}/share/picmi/gfx/\")

configure_file(
	"${PROJECT_SOURCE_DIR}/src/config.h.in"
	"${PROJECT_BINARY_DIR}/config.h"
)

find_package(Qt4 REQUIRED)
find_package(Boost REQUIRED)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -O3 -pedantic -fPIC -std=c++0x")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} -g -Wall -Wextra -O3 -pedantic -fPIC -std=c++0x")

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

set(picmi_FORMS
	${CMAKE_SOURCE_DIR}/src/gui/helpwindow.ui
	${CMAKE_SOURCE_DIR}/src/gui/highscorewindow.ui
	${CMAKE_SOURCE_DIR}/src/gui/mainwindow.ui
	${CMAKE_SOURCE_DIR}/src/gui/settingswindow.ui
)
qt4_wrap_ui(picmi_FORMS_HEADERS ${picmi_FORMS})

set(picmi_QTHEADERS
	${CMAKE_SOURCE_DIR}/src/gui/helpwindow.h
	${CMAKE_SOURCE_DIR}/src/gui/highscorewindow.h
	${CMAKE_SOURCE_DIR}/src/gui/mainwindow.h
	${CMAKE_SOURCE_DIR}/src/gui/settingswindow.h
	${CMAKE_SOURCE_DIR}/src/gui/scene.h
)
qt4_wrap_cpp(picmi_QTHEADERS_MOC ${picmi_QTHEADERS})


include_directories(
	${CMAKE_SOURCE_DIR}
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
)
aux_source_directory(src/ src_sources)
aux_source_directory(src/gui src_gui_sources)
aux_source_directory(src/logic src_logic_sources)

add_executable(picmi
	${src_sources}
	${src_gui_sources}
	${src_logic_sources}
	${picmi_FORMS_HEADERS}
	${picmi_QTHEADERS_MOC}
)

target_link_libraries(picmi
	${QT_LIBRARIES}
)

install(TARGETS picmi DESTINATION bin)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/gfx DESTINATION share/picmi)
install(FILES ${CMAKE_SOURCE_DIR}/picmi.desktop DESTINATION share/applications/)
