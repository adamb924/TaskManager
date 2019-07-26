#ifndef LISTVIEW_H
#define LISTVIEW_H

#include <QTreeView>
#include <QUrl>

class QStandardItemModel;
class QStandardItem;

class ListView : public QTreeView
{
    Q_OBJECT

public:
    explicit ListView(QWidget *parent = nullptr);

signals:
    void showArchive();
    void preferences();
    void save();
    void archiveItem( QStandardItem * );

protected slots:
    void insert(QUrl url = QUrl());
    void insertLink();
    void insertSubItem();
    void insertSubItemLink();
    void insertHyperlink();
    void removeHyperlink();
    void remove();
    void archive();
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent* event);

protected:
    void contextMenuEvent(QContextMenuEvent *e);
    QStandardItem * getCurrentItem();
    void mouseDoubleClickEvent(QMouseEvent *event);
};

#endif // LISTVIEW_H
