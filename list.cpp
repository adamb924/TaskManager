#include "list.h"

#include <QStandardItemModel>
#include <QTreeView>

#include "itemproxymodel.h"
#include "listview.h"

List::List(ListView *v, const QString &xml) : model(new QStandardItemModel), proxy(new ItemProxyModel), view(v), xmlString(xml)
{
    model->setObjectName(xmlString);
    proxy->setObjectName(xmlString);
    proxy->setSourceModel( model );
    view->setModel( proxy );
}

List::List(const List &other) : model( other.model ), proxy( other.proxy ), view( other.view ), xmlString( other.xmlString )
{
}

List::~List()
{
    delete model;
    delete proxy;
}

List &List::operator=(const List &other)
{
    model = other.model;
    proxy = other.proxy;
    view = other.view;
    xmlString = other.xmlString;
    return *this;
}

QModelIndex List::indexFromItem(QStandardItem *item)
{
    return proxy->mapFromSource( model->indexFromItem(item) );
}
