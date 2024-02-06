QT       += core gui widgets

TARGET = TaskManager
TEMPLATE = app

INCLUDEPATH += tasks/
INCLUDEPATH += macrotasks/

SOURCES += main.cpp\
    events/event.cpp \
    events/eventdayfilter.cpp \
    events/eventeditdialog.cpp \
    events/eventitemmodel.cpp \
    events/eventview.cpp \
    macrotasks/macrotask.cpp \
    macrotasks/macrotaskdialog.cpp \
    macrotasks/macrotaskitemdelegate.cpp \
    macrotasks/macrotasklistmodel.cpp \
    macrotasks/macrotaskview.cpp \
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
    macrotasks/macrotask.h \
    macrotasks/macrotaskdialog.h \
    macrotasks/macrotaskitemdelegate.h \
    macrotasks/macrotasklistmodel.h \
    macrotasks/macrotaskview.h \
    tasks/filterwidget.h \
    tasks/itemproxymodel.h \
    tasks/linkeditdialog.h \
    tasks/list.h \
    tasks/listview.h

FORMS += \
    events/eventeditdialog.ui \
    macrotasks/macrotaskdialog.ui \
    tasks/filterwidget.ui \
    tasks/linkeditdialog.ui \
    mainwindow.ui \
    tasks/archive.ui \
    preferencesdialog.ui
RC_ICONS = icons/TaskManager.ico
