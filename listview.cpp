#include "listview.h"
#include "mainwindow.h"
#include "itemproxymodel.h"

#include <QMenu>
#include <QStandardItemModel>
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QDate>
#include <QTime>
#include <QInputDialog>
#include <QDesktopServices>
#include <QGuiApplication>

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
        menu.addSeparator();
        menu.addAction(tr("Insert/edit link"), this,SLOT(insertHyperlink()) );
        menu.addAction(tr("Remove link"), this,SLOT(removeHyperlink()) );
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

QStandardItem *ListView::getCurrentItem()
{
    ItemProxyModel * ipm = qobject_cast<ItemProxyModel *>(model());
    QStandardItemModel * m = qobject_cast<QStandardItemModel *>(ipm->sourceModel());
    QModelIndexList selection = selectedIndexes();
    if( selection.isEmpty() ) { return nullptr; }
    return m->itemFromIndex( ipm->mapToSource( selection.first() ) );
}

void ListView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if( Qt::ControlModifier & QGuiApplication::keyboardModifiers() )
    {
        ItemProxyModel * ipm = qobject_cast<ItemProxyModel *>(model());
        QStandardItemModel * m = qobject_cast<QStandardItemModel *>(ipm->sourceModel());
        QStandardItem * item = m->itemFromIndex( ipm->mapToSource( indexAt( event->pos() ) ) );

        if( item != nullptr )
        {
            QString url = item->data(MainWindow::Url).toString();
            if( !url.isEmpty() )
            {
                QDesktopServices::openUrl( QUrl::fromUserInput( url ) );
            }
        }
    }
    else
    {
        QTreeView::mouseDoubleClickEvent(event);
    }
}

void ListView::archive()
{
    ItemProxyModel * ipm = qobject_cast<ItemProxyModel *>(model());
    QStandardItemModel * m = qobject_cast<QStandardItemModel *>(ipm->sourceModel());
    QModelIndexList selection = selectedIndexes();
    if( selection.isEmpty() ) { return; }
    QModelIndex selected = ipm->mapToSource( selection.first() );
    QStandardItem *item = m->itemFromIndex( selected );

    item->setData( QDate::currentDate() , MainWindow::Date );

    if( selection.first().parent() != QModelIndex() )
    {
        QStandardItem *parent = m->itemFromIndex( selected.parent() );
        parent->takeRow( selected.row() );
    }
    else
    {
        m->takeRow( selected.row() );
    }
    mArchive->appendRow(item);
}

void ListView::insert()
{
    ItemProxyModel * ipm = qobject_cast<ItemProxyModel *>(model());
    QStandardItemModel * m = qobject_cast<QStandardItemModel *>(ipm->sourceModel());
    QStandardItem *item = mMainWnd->newItem(false, "", mDateFormat);
    m->appendRow( item ) ;
    edit( ipm->mapFromSource( m->indexFromItem(item) ) );
}

void ListView::insertSubItem()
{
    ItemProxyModel * ipm = qobject_cast<ItemProxyModel *>(model());
    QStandardItemModel * m = qobject_cast<QStandardItemModel *>(ipm->sourceModel());
    QModelIndexList selection = selectedIndexes();
    if( selection.isEmpty() ) { return; }
    QStandardItem *parent = m->itemFromIndex( ipm->mapToSource( selection.first() ) );

    QStandardItem *item = mMainWnd->newItem(false, "", mDateFormat);
    expand(selection.first());
    parent->appendRow(item);
    edit( ipm->mapFromSource( m->indexFromItem(item) ) );
}

void ListView::insertHyperlink()
{
    QStandardItem *item = getCurrentItem();
    if( item != nullptr )
    {
        QString url = QInputDialog::getText(this, tr("Enter URL"), tr("Enter the link:") );
        if( !url.isNull() )
        {
            item->setData( url , MainWindow::Url );
        }
    }
}

void ListView::removeHyperlink()
{
    QStandardItem *item = getCurrentItem();
    if( item != nullptr )
    {
        item->setData( QString() , MainWindow::Url );
    }
}

void ListView::remove()
{
    ItemProxyModel * ipm = qobject_cast<ItemProxyModel *>(model());
    QStandardItemModel * m = qobject_cast<QStandardItemModel *>(ipm->sourceModel());
    QModelIndexList selection = selectedIndexes();
    foreach( QModelIndex index , selection )
    {
        if( ipm->mapToSource( selection.first() ).parent() != QModelIndex() )
        {
            QStandardItem *parent = m->itemFromIndex( ipm->mapToSource( index.parent() ) );
            parent->removeRow( index.row() );
        }
        else
        {
            m->removeRow( index.row() );
        }
    }
}
