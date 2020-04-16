#-------------------------------------------------
#
# Project created by QtCreator 2011-01-16T14:24:32
#
#-------------------------------------------------

QT       += core gui widgets sql

TARGET = TaskManager
TEMPLATE = app


SOURCES += main.cpp\
    event.cpp \
    eventdayfilter.cpp \
    eventeditdialog.cpp \
    eventitemmodel.cpp \
    eventview.cpp \
    filterwidget.cpp \
    itemproxymodel.cpp \
    linkeditdialog.cpp \
    list.cpp \
        mainwindow.cpp \
    listview.cpp

HEADERS  += mainwindow.h \
    event.h \
    eventdayfilter.h \
    eventeditdialog.h \
    eventitemmodel.h \
    eventview.h \
    filterwidget.h \
    itemproxymodel.h \
    linkeditdialog.h \
    list.h \
    listview.h

FORMS += \
    eventeditdialog.ui \
    filterwidget.ui \
    linkeditdialog.ui \
    mainwindow.ui \
    archive.ui \
    preferencesdialog.ui
RC_ICONS = icons/TaskManager.ico
