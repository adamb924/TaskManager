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
    linkeditdialog.cpp \
    list.cpp \
        mainwindow.cpp \
    listview.cpp

HEADERS  += mainwindow.h \
    itemproxymodel.h \
    linkeditdialog.h \
    list.h \
    listview.h

FORMS += \
    linkeditdialog.ui \
    mainwindow.ui \
    archive.ui \
    preferencesdialog.ui
RC_ICONS = icons/TaskManager.ico
