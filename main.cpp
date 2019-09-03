#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QSharedMemory sharedMemory;
    sharedMemory.setKey("com.adambaker.taskmanager");

    QApplication a(argc, argv);

    if (!sharedMemory.create(1))
    {
        QMessageBox::warning(nullptr, QObject::tr("Warning!"), QObject::tr("Task Manager is already running. Opening multiple instances of the program can result in your data being lost.") );
        return 0;
    }

    MainWindow w;
    w.show();

    return a.exec();
}
