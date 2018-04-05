#-------------------------------------------------
#
# Project created by QtCreator 2018-04-03T23:52:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = smallMetexLab
TEMPLATE = app


SOURCES += main.cpp\
        smallmetexlab.cpp \
    serialport.cpp \
    analogdisplay.cpp \
    qcustomplot.cpp

HEADERS  += smallmetexlab.h \
    serialport.h \
    analogdisplay.h \
    qcustomplot.h

FORMS    += smallmetexlab.ui
