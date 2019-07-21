#ifndef LIST_H
#define LIST_H

#include <QString>
#include <QModelIndex>

class QStandardItemModel;
class ItemProxyModel;
class ListView;
class QStandardItem;

class List {
public:
    List( ListView *v, const QString &xml );

    List (const List &other);

    ~List();

    List& operator=(const List& other);

    QModelIndex indexFromItem(QStandardItem * item);

private:
    QStandardItemModel * model;
    ItemProxyModel * proxy;
    ListView * view;
    QString xmlString;

    friend class MainWindow;
};

#endif // LIST_H
