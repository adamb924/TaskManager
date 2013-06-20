#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class ListWidget;
class ArchiveWidget;
class QSettings;
class QDialog;
class QDockWidget;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void showArchive();
    void preferences();
    void saveData();
    void removeAllFromArchive();

private:
    QSettings *settings;
    QLabel *ul, *ur, *ll, *lr;
    QString dateFormat, timeFormat;

    ListWidget *urgentImportant, *urgentNotImportant, *notUrgentImportant, *notUrgentNotImportant;
    ArchiveWidget *archive;
//    QDialog *archiveDialog;
    QDockWidget *archiveDock;

    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
