#-------------------------------------------------
#
# Project created by QtCreator 2011-01-16T14:24:32
#
#-------------------------------------------------

QT       += core gui widgets sql

TARGET = TaskManager
TEMPLATE = app


SOURCES += main.cpp\
    itemproxymodel.cpp \
        mainwindow.cpp \
    listview.cpp

HEADERS  += mainwindow.h \
    itemproxymodel.h \
    listview.h

FORMS += \
    mainwindow.ui \
    archive.ui \
    preferencesdialog.ui
