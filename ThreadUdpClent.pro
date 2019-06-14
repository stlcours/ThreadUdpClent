#-------------------------------------------------
#
# Project created by QtCreator 2019-5-29T11:04:33
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ThreadUdpClent
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_MESSAGELOGCONTEXT
DEFINES += QT_DEPRECATED_WARNINGS

# Output directory
CONFIG(debug, debug|release) {
    output = debug
    TARGET = template-qt_d
}
CONFIG(release, debug|release) {
    output = release
}

DESTDIR     = bin
OBJECTS_DIR = $$output
MOC_DIR     = $$output
RCC_DIR     = $$output
UI_DIR      = $$output

RESOURCES +=

RC_FILE = app.rc

DISTFILES += \
    util/util.pri \
    data/config.json

#INCLUDEPATH += gui
#DEPENDPATH += gui

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(gui/gui.pri)
include(util/util.pri)
include(3rdParty/qtsingleapplication/qtsingleapplication.pri)
include(3rdParty/qxtglobalshortcut/qxtglobalshortcut.pri)

TRANSLATIONS = ThreadSerial_Template_us.ts ThreadSerial_Template_cn.ts


HEADERS += \
    console.h \
    global.h \
    widgetdatacontrol.h \
    udpwork.hpp


SOURCES += \
    console.cpp \
    global.cpp \
    main.cpp \
    widgetdatacontrol.cpp


FORMS +=


