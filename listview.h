#ifndef LISTVIEW_H
#define LISTVIEW_H

#include <QTreeView>

class QStandardItemModel;
class QStandardItem;
class MainWindow;

class ListView : public QTreeView
{
    Q_OBJECT

public:
    explicit ListView(QWidget *parent = nullptr);

    void setArchive(QStandardItemModel *archive);
    void setDateTimeFormat(const QString & date);

signals:
    void showArchive();
    void preferences();
    void save();

protected slots:
    void insert();
    void insertSubItem();
    void insertHyperlink();
    void removeHyperlink();
    void remove();
    void archive();

protected:
    bool isArchive;
    void contextMenuEvent(QContextMenuEvent *e);
    QStandardItem * getCurrentItem();
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    QStandardItemModel * mArchive;
    QString mDateFormat;
    const MainWindow * mMainWnd;
};

#endif // LISTVIEW_H
