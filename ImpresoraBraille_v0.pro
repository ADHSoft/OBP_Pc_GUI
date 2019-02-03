#-------------------------------------------------
#
# Project created by QtCreator 2017-11-29T21:02:28
#
#-------------------------------------------------

QT += core  gui  serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ImpresoraBraille_v0
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ayuda.cpp

HEADERS  += mainwindow.h \
    ayuda.h

FORMS    += mainwindow.ui \
    ayuda.ui

RESOURCES += \
    recursos.qrc
