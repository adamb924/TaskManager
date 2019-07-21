#include "list.h"

#include <QStandardItemModel>
#include <QTreeView>

#include "itemproxymodel.h"
#include "listview.h"

List::List(ListView *v, const QString &xml) : model(new QStandardItemModel), proxy(new ItemProxyModel), view(v), xmlString(xml)
{
    proxy->setSourceModel( model );
    view->setModel( proxy );
}

List::List(const List &other)
{
    model = other.model;
    proxy = other.proxy;
    view = other.view;
}

List::~List()
{
    delete model;
    delete proxy;
}

QModelIndex List::indexFromItem(QStandardItem *item)
{
    return proxy->mapFromSource( model->indexFromItem(item) );
}
