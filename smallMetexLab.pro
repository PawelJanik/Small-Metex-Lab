#-------------------------------------------------
#
# Project created by QtCreator 2018-04-03T23:52:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = smallMetexLab
TEMPLATE = app


SOURCES += main.cpp\
        smallmetexlab.cpp \
    serialport.cpp

HEADERS  += smallmetexlab.h \
    serialport.h

FORMS    += smallmetexlab.ui
