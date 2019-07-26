#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QDateTime>
#include <QDir>
#include <QUrl>

class QDialog;
class QDockWidget;
class QLabel;
class QXmlStreamWriter;
class QTreeView;
class ItemProxyModel;
class List;

namespace Ui {
    class MainWindow;
    class Archive;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    enum ListType { UrgentImportant, NotUrgentImportant, UrgentNotImportant, NotUrgentNotImportant, Archive };
    enum DataType { Label = Qt::UserRole+1, Date, JustChanged, Url, Completed };
    static QStandardItem* newItem(bool checked, const QString & label, const QDateTime & date = QDateTime(), const QUrl &url = QString() );

public slots:
    void showArchive();
    void preferences();
    bool writeXmlData(QString path = QString());
    void removeAllFromArchive();

private slots:
    void openFile();
    void openDataDirectory();
    void saveAs();
    void archiveItem(QStandardItem *item);
    void itemChanged(QStandardItem *item);

private:
    Ui::MainWindow *ui;
    Ui::Archive *archiveUi;

    QDir mDataFolder;

    QString mDateFormat;

    QDockWidget *mArchiveDock;

    QHash<MainWindow::ListType, List*> mLists;

    void closeEvent(QCloseEvent *event);
    void serializeModel(List * list, QXmlStreamWriter *stream) const;
    void serializeItem(List * list, QStandardItem * item, QXmlStreamWriter *stream) const;
    void propagateDateTime();
    void readXmlData(QString path = QString());
    QString dataFileReadPath() const;
    QString dataFileWritePath() const;
    void cleanUpOldCopies();
};

#endif // MAINWINDOW_H
