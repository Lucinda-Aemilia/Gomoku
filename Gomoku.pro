#-------------------------------------------------
#
# Project created by QtCreator 2016-08-30T17:48:59
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Gomoku
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    board.cpp \
    createdialog.cpp \
    connectdialog.cpp

HEADERS  += mainwindow.h \
    board.h \
    createdialog.h \
    connectdialog.h

FORMS    += mainwindow.ui \
    board.ui \
    createdialog.ui \
    connectdialog.ui
