#-------------------------------------------------
#
# Project created by QtCreator 2011-01-16T14:24:32
#
#-------------------------------------------------

QT       += core gui widgets xml

TARGET = TaskManager
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    listview.cpp \
    taskitem.cpp

HEADERS  += mainwindow.h \
    listview.h \
    taskitem.h

FORMS += \
    mainwindow.ui \
    archive.ui \
    preferencesdialog.ui
