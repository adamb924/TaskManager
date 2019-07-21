#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QDateTime>
#include <QDir>

class QDialog;
class QDockWidget;
class QLabel;
class QXmlStreamWriter;
class QTreeView;
class ItemProxyModel;

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

    enum DataType { Label = Qt::UserRole+1, Date, JustChanged, Url };
    static QStandardItem* newItem(bool checked, const QString & label, const QString & dateFormat, const QDateTime & date = QDateTime(), const QString & url = QString() );

protected:
    void contextMenuEvent(QContextMenuEvent *event);

public slots:
    void showArchive();
    void preferences();
    bool writeXmlData(QString path = QString());
    void removeAllFromArchive();

    void itemChanged(QStandardItem * item);

private slots:
    void openFile();
    void openDataDirectory();
    void saveAs();

private:
    Ui::MainWindow *ui;
    Ui::Archive *archiveUi;

    QDir mDataFolder;

    QString mDateFormat;

    QStandardItemModel mUrgentImportant, mUrgentNotImportant, mNotUrgentImportant, mNotUrgentNotImportant, mArchive;

    ItemProxyModel * mUrgentImportantProxy, * mUrgentNotImportantProxy, * mNotUrgentImportantProxy, * mNotUrgentNotImportantProxy, * mArchiveProxy;

    QDockWidget *mArchiveDock;


    void closeEvent(QCloseEvent *event);
    void addItemsToModel(const QString & string, QStandardItemModel *model) const;
    void serializeModel(QStandardItemModel * model, QXmlStreamWriter *stream, QTreeView *view, ItemProxyModel *proxy) const;
    void serializeItem(QStandardItemModel *model, QStandardItem * item, QXmlStreamWriter *stream, QTreeView *view, ItemProxyModel *proxy) const;
    void propagateDateTime();
    void readXmlData(QString path = QString());
    QString dataFileReadPath() const;
    QString dataFileWritePath() const;
    void cleanUpOldCopies();
};

#endif // MAINWINDOW_H
