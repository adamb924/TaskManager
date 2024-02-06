#-------------------------------------------------
#
# Project created by QtCreator 2011-01-16T14:24:32
#
#-------------------------------------------------

QT       += core gui widgets sql

TARGET = TaskManager
TEMPLATE = app


SOURCES += main.cpp\
    events/event.cpp \
    events/eventdayfilter.cpp \
    events/eventeditdialog.cpp \
    events/eventitemmodel.cpp \
    events/eventview.cpp \
    tasks/filterwidget.cpp \
    tasks/itemproxymodel.cpp \
    tasks/linkeditdialog.cpp \
    tasks/list.cpp \
        mainwindow.cpp \
    tasks/listview.cpp

HEADERS  += mainwindow.h \
    events/event.h \
    events/eventdayfilter.h \
    events/eventeditdialog.h \
    events/eventitemmodel.h \
    events/eventview.h \
    tasks/filterwidget.h \
    tasks/itemproxymodel.h \
    tasks/linkeditdialog.h \
    tasks/list.h \
    tasks/listview.h

FORMS += \
    events/eventeditdialog.ui \
    tasks/filterwidget.ui \
    tasks/linkeditdialog.ui \
    mainwindow.ui \
    tasks/archive.ui \
    preferencesdialog.ui
RC_ICONS = icons/TaskManager.ico
