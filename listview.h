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
    void archiveItem( QStandardItem *, const QString &);
    void openFile();
    void openDataDirectory();
    void saveAs();

protected slots:
    void insert(QUrl url = QUrl());
    void insertLink();
    void insertSubItem();
    void insertSubItemLink();
    void insertHyperlink();
    void removeHyperlink();
    void remove();
    void archive();
    void copyItem();
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent* event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void openLink(const QPoint &pos);
    void togglePutOnHold();

protected:
    void contextMenuEvent(QContextMenuEvent *e);
    QStandardItem * getSelectedItem();
    QStandardItem *getItem(const QPoint &pos);
    void mouseDoubleClickEvent(QMouseEvent *event);
    QString getItemText(QStandardItem * item, const QString & subitemPrefix = QString());
};

#endif // LISTVIEW_H
