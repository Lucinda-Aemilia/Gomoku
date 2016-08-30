#-------------------------------------------------
#
# Project created by QtCreator 2016-08-30T17:48:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Gomoku
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    board.cpp

HEADERS  += mainwindow.h \
    board.h

FORMS    += mainwindow.ui \
    board.ui
