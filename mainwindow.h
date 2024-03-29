#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QDateTime>
#include <QDir>
#include <QUrl>

#include "macrotasks/macrotask.h"

class QDialog;
class QDockWidget;
class QLabel;
class QXmlStreamWriter;
class QTreeView;
class ItemProxyModel;
class List;
class FilterWidget;
class Event;
class EventItemModel;
class EventView;
class MacrotaskListModel;

namespace Ui {
    class MainWindow;
    class Archive;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    enum ListType { UrgentImportant, NotUrgentImportant, UrgentNotImportant, NotUrgentNotImportant, Archive };
    enum DataType { Label = Qt::UserRole+1, DateCompleted, JustChanged, Url, DateCreated, OriginList, PutOnHold };
    static QStandardItem* newItem(bool checked, const QString & label, const QDateTime & dateCreated = QDateTime(), const QDateTime & dateCompleted = QDateTime(), const QUrl &url = QString() );

    void setupFilterDock();

public slots:
    void showArchive();
    void preferences();
    bool writeXmlData(QString path = QString());
    void removeAllFromArchive();

private slots:
    void openFile();
    void openDataDirectory();
    void saveAs();
    void archiveItem(QStandardItem *item, const QString & origin);
    void itemChanged(QStandardItem *item);
    void toggleDisplayFilterWindow();

    void setCaseSensitiveFiltering(int checkState);
    void checkForNoMatches();

private:
    Ui::MainWindow *ui;
    Ui::Archive *archiveUi;

    QDir mDataFolder;

    QString mDateFormat;

    QList<int> mEventDivisions;

    QDockWidget *mArchiveDock;
    FilterWidget * mFilterWidget;
    QDockWidget * mFilterDock;

    QHash<MainWindow::ListType, List*> mLists;

    QList<Event*> mEvents;
    EventItemModel * mEventModel;
    QList<EventView*> mEventViews;

    MacrotaskListModel * mMacrotaskModel;
    QList<Macrotask> mMacrotasks;

    bool mShowEvents;
    bool mShowMacrotasks;

    void closeEvent(QCloseEvent *event) override;
    void serializeModel(List * list, QXmlStreamWriter *stream) const;
    void serializeItem(List * list, QStandardItem * item, QXmlStreamWriter *stream) const;
    void serializeEvent(Event * e, QXmlStreamWriter *stream) const;
    void serializeMacrotask(const Macrotask & m, QXmlStreamWriter *stream) const;
    void propagateDateTime();
    void readXmlData(QString path = QString());
    QString dataFileReadPath() const;
    QString dataFileWritePath() const;
    void cleanUpOldCopies();

    /// Event divisions
    QString eventDivisionsString() const;
    void setEventDivisionsFromString(const QString & string);
    void setupEventSplitters();
};

#endif // MAINWINDOW_H
