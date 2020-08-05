#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>
#include <QObject>
#include "mainwindow.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
    QSharedMemory sharedMemory;
    sharedMemory.setKey("com.adambaker.taskmanager");

    QApplication a(argc, argv);

    if (!sharedMemory.create(1)) /// if some other instance of this application is already running
    {
        #ifdef _WIN32
        /// in Windows, raise the window called "Task Manager" and then quit
        HWND hWnd = ::FindWindow(nullptr, reinterpret_cast<LPCWSTR>( QObject::tr("Task Manager").utf16() ) );
        if (hWnd) {
            // move to foreground
            ::SetForegroundWindow(hWnd);
        }

        #else
        /// I don't know how to do it in other OS's, so just give a nagging message
        QMessageBox::warning(nullptr, QObject::tr("Warning!"), QObject::tr("Task Manager is already running. Opening multiple instances of the program can result in your data being lost.") );
        #endif

        return 0;
    }

    MainWindow w;
    w.show();

    return a.exec();
}
