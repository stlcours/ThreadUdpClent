
QT += gui
QT += core
QT += network
QT += widgets

INCLUDEPATH += $$PWD/

SOURCES += \
    $$PWD/ctabwidget.cpp \
    $$PWD/aequilatetabbar.cpp

HEADERS += \
    $$PWD/ctabwidget.h \
    $$PWD/aequilatetabbar.h

#需要提升类

RESOURCES += $$PWD/rc.qrc
