#-------------------------------------------------
#
# Project created by QtCreator 2018-11-06T15:55:11
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += multimedia
QT       += multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Tcp_KeHuDuan
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        client.cpp \
    room.cpp \
    index.cpp \
    videosuface.cpp \
    screen.cpp \
    danmu.cpp

HEADERS  += client.h \
    package.h \
    room.h \
    index.h \
    videosuface.h \
    screen.h \
    danmu.h

FORMS    += client.ui \
    room.ui \
    index.ui \
    screen.ui
