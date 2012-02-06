#-------------------------------------------------
#
# Project created by QtCreator 2012-02-06T14:36:06
#
#-------------------------------------------------

QT       += core gui svg

TARGET = picmi
TEMPLATE = app

SOURCES += src/systemexception.cpp \
    src/settings.cpp \
    src/outofboundsexception.cpp \
    src/main.cpp \
    src/logic/picmi.cpp \
    src/logic/boardstate.cpp \
    src/logic/boardmap.cpp \
    src/logic/board.cpp \
    src/gui/view.cpp \
    src/gui/streakitem.cpp \
    src/gui/scene.cpp \
    src/gui/renderer.cpp \
    src/gui/reloadableitem.cpp \
    src/gui/pixmapitem.cpp \
    src/gui/mainwindow.cpp \
    src/gui/dragmanager.cpp \
    src/gui/cellitem.cpp \
    src/gui/highlightitem.cpp \
    src/gui/helpwindow.cpp \
    src/gui/settingswindow.cpp \
    src/logic/elapsedtime.cpp \
    src/gui/pausebanneritem.cpp

HEADERS  += src/settings.h \
    src/outofboundsexception.h \
    src/systemexception.h \
    src/logic/picmi.h \
    src/logic/boardstate.h \
    src/logic/boardmap.h \
    src/logic/board.h \
    src/gui/view.h \
    src/gui/streakitem.h \
    src/gui/scene.h \
    src/gui/renderer.h \
    src/gui/reloadableitem.h \
    src/gui/pixmapitem.h \
    src/gui/mainwindow.h \
    src/gui/graphicsitem.h \
    src/gui/dragmanager.h \
    src/gui/cellitem.h \
    src/gui/highlightitem.h \
    src/config.h \
    src/gui/helpwindow.h \
    src/gui/settingswindow.h \
    src/logic/elapsedtime.h \
    src/gui/pausebanneritem.h \
    src/constants.h

FORMS    += src/gui/mainwindow.ui \
    src/gui/helpwindow.ui \
    src/gui/settingswindow.ui
