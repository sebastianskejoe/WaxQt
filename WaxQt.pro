#-------------------------------------------------
#
# Project created by QtCreator 2014-01-24T15:49:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = WaxQt
TEMPLATE = app

SOURCES += main.cpp\
        waxqt.cpp \
    transmitterdialog.cpp \
    listdevicesdialog.cpp \
    waxrec.c \
    loggerworker.cpp \
    qcustomplot.cpp \
    device.cpp \
    exportdialog.cpp

HEADERS  += waxqt.h \
    transmitterdialog.h \
    listdevicesdialog.h \
    waxrec.h \
    loggerworker.h \
    qcustomplot.h \
    device.h \
    exportdialog.h

FORMS    += waxqt.ui \
    newtransmitter.ui \
    listdevices.ui \
    exportdialog.ui
