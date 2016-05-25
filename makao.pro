#-------------------------------------------------
#
# Project created by QtCreator 2016-04-16T20:41:13
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = makao
TEMPLATE = app


SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/cardswidget.cpp \
    src/singlecardwidget.cpp \
    src/card.cpp \
    src/client.cpp \
    src/player.cpp \
    src/server.cpp \
    src/lobbywidget.cpp \
    src/gamescreenwidget.cpp \
    src/server.cpp \
    src/table.cpp \
    src/opponentcardswidget.cpp \
    src/utils.cpp

HEADERS  += src/mainwindow.h \
    src/cardswidget.h \
    src/singlecardwidget.h \
    src/card.h \
    src/client.h \
    src/messagetype.h \
    src/player.h \
    src/server.h \
    src/lobbywidget.h \
    src/gamescreenwidget.h \
    src/server.h \
    src/table.h \
    src/opponentcardswidget.h \
    src/utils.h

FORMS    += src/mainwindow.ui \
    src/cardswidget.ui \
    src/singlecardwidget.ui \
    src/lobbywidget.ui \
    src/gamescreenwidget.ui \
    src/opponentcardswidget.ui

RESOURCES += \
    tlomenu.qrc \
    gamescreen.qrc
