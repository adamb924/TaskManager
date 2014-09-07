#include "listview.h"

#include <QMenu>
#include <QStandardItemModel>
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QDate>
#include <QTime>

#include <QtDebug>

ListView::ListView(QWidget *parent) : QTreeView(parent)
{
}

void ListView::setArchive(QStandardItemModel *archive)
{
    mArchive = archive;
}

void ListView::setDateTimeFormat(const QString &date)
{
    mDateFormat = date;
}

void ListView::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu menu;
    if( indexAt(e->pos()).isValid() )
    {
        menu.addAction( tr("Delete this item"), this,SLOT(remove()) );
        menu.addAction(tr("Archive this item"), this,SLOT(archive()));
        menu.addAction(tr("Insert subitem"), this,SLOT(insertSubItem()) );
    }
    else
    {
        menu.addAction(tr("Insert an item"), this,SLOT(insert()) );
    }
    menu.addSeparator();
    menu.addAction(tr("Show the archive"), this,SIGNAL(showArchive()) );
    menu.addAction(tr("Preferences"), this,SIGNAL(preferences()) );
    menu.addAction(tr("Save"), this,SIGNAL(save()) );
    menu.exec(e->globalPos());
}

void ListView::archive()
{
    QStandardItemModel * m = qobject_cast<QStandardItemModel *>(model());
    QModelIndexList selection = selectedIndexes();
    if( selection.isEmpty() ) { return; }
    QStandardItem *item = m->itemFromIndex( selection.first() );

    item->setData( QDate::currentDate() );

    if( selection.first().parent() != QModelIndex() )
    {
        QStandardItem *parent = m->itemFromIndex( selection.first().parent() );
        parent->takeRow( selection.first().row() );
    }
    else
    {
        m->takeRow( selection.first().row() );
    }
    mArchive->appendRow(item);
}

void ListView::insert()
{
    QStandardItemModel * m = qobject_cast<QStandardItemModel *>(model());
    QStandardItem *item = new QStandardItem( "" );
    item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled );
    item->setCheckState(Qt::Unchecked);
    m->appendRow(item);
    edit( m->indexFromItem(item) );
}

void ListView::insertSubItem()
{
    QStandardItemModel * m = qobject_cast<QStandardItemModel *>(model());
    QModelIndexList selection = selectedIndexes();
    if( selection.isEmpty() ) { return; }
    QStandardItem *parent = m->itemFromIndex( selection.first() );

    QStandardItem *item = new QStandardItem( "" );
    item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled );
    item->setCheckState(Qt::Unchecked);
    expand(selection.first());
    parent->appendRow(item);
    edit( m->indexFromItem(item) );
}

void ListView::remove()
{
    QStandardItemModel * m = qobject_cast<QStandardItemModel *>(model());
    QModelIndexList selection = selectedIndexes();
    foreach( QModelIndex index , selection )
    {
        if( selection.first().parent() != QModelIndex() )
        {
            QStandardItem *parent = m->itemFromIndex( index.parent() );
            parent->removeRow( index.row() );
        }
        else
        {
            m->removeRow( index.row() );
        }
    }
}
