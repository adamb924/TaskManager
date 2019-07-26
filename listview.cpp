#include "listview.h"
#include "mainwindow.h"
#include "itemproxymodel.h"
#include "linkeditdialog.h"

#include <QMenu>
#include <QStandardItemModel>
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QDate>
#include <QTime>
#include <QInputDialog>
#include <QDesktopServices>
#include <QGuiApplication>
#include <QMimeData>

#include <QtDebug>

ListView::ListView(QWidget *parent) : QTreeView(parent)
{
    setAcceptDrops(true);
    setMouseTracking(true);
}

void ListView::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu menu;
    if( indexAt(e->pos()).isValid() )
    {
        menu.addAction( tr("Delete this item"), this,SLOT(remove()) );
        menu.addAction(tr("Archive this item"), this,SLOT(archive()));
        menu.addAction(tr("Insert subitem"), this,SLOT(insertSubItem()) );
        menu.addAction(tr("Insert link as subitem"), this,SLOT(insertSubItemLink()) );
        menu.addSeparator();
        menu.addAction(tr("Insert/edit link"), this,SLOT(insertHyperlink()) );
        menu.addAction(tr("Remove link"), this,SLOT(removeHyperlink()) );
    }
    else
    {
        menu.addAction(tr("Insert an item"), this,SLOT(insert()) );
        menu.addAction(tr("Insert a link"), this,SLOT(insertLink()) );
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
            QUrl url = item->data(MainWindow::Url).toUrl();
            if( !url.isEmpty() )
            {
                QDesktopServices::openUrl( url );
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
//    mArchive->appendRow(item);
    emit archiveItem(item);
}

void ListView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
    else
    {
        QTreeView::dragEnterEvent(event);
    }
}

void ListView::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
    else
    {
        QTreeView::dragMoveEvent(event);
    }
}

void ListView::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        QModelIndex index = indexAt( event->pos() );
        ItemProxyModel * ipm = qobject_cast<ItemProxyModel *>(model());
        QStandardItemModel * m = qobject_cast<QStandardItemModel *>(ipm->sourceModel());
        QStandardItem *item = m->itemFromIndex( ipm->mapToSource( index ) );
        if( item == nullptr )
        {
            insert( event->mimeData()->urls().at(0) );
        }
        else
        {
            item->setData( event->mimeData()->urls().at(0), MainWindow::Url );
        }
    }
    else
    {
        QTreeView::dropEvent(event);
    }
}

void ListView::mouseMoveEvent(QMouseEvent *event)
{
    if( QGuiApplication::keyboardModifiers() != Qt::ControlModifier )
    {
        setCursor(Qt::ArrowCursor);
        QTreeView::mouseMoveEvent(event);
        return;
    }
    QModelIndex index = indexAt( event->pos() );
    if( index.isValid() )
    {
        ItemProxyModel * ipm = qobject_cast<ItemProxyModel *>(model());
        QStandardItemModel * m = qobject_cast<QStandardItemModel *>(ipm->sourceModel());
        QStandardItem *item = m->itemFromIndex( ipm->mapToSource( index ) );
        if( !item->data(MainWindow::Url).toUrl().isEmpty() )
        {
            setCursor(Qt::PointingHandCursor);
        }
        else
        {
            setCursor(Qt::ArrowCursor);
        }
        QTreeView::mouseMoveEvent(event);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
        QTreeView::mouseMoveEvent(event);
    }
}

void ListView::insert(QUrl url)
{
    ItemProxyModel * ipm = qobject_cast<ItemProxyModel *>(model());
    QStandardItemModel * m = qobject_cast<QStandardItemModel *>(ipm->sourceModel());
    QStandardItem *item = MainWindow::newItem(false, "");
    if( !url.isEmpty() )
    {
        item->setData( url, MainWindow::Url );
    }
    m->appendRow( item );
    edit( ipm->mapFromSource( m->indexFromItem(item) ) );
}

void ListView::insertLink()
{
    ItemProxyModel * ipm = qobject_cast<ItemProxyModel *>(model());
    QStandardItemModel * m = qobject_cast<QStandardItemModel *>(ipm->sourceModel());
    LinkEditDialog dlg;
    if( dlg.exec() == QDialog::Accepted )
    {
        QStandardItem *item = MainWindow::newItem(false, dlg.label(), QDateTime(), dlg.url());
        m->appendRow( item );
    }
}

void ListView::insertSubItem()
{
    ItemProxyModel * ipm = qobject_cast<ItemProxyModel *>(model());
    QStandardItemModel * m = qobject_cast<QStandardItemModel *>(ipm->sourceModel());
    QModelIndexList selection = selectedIndexes();
    if( selection.isEmpty() ) { return; }
    QStandardItem *parent = m->itemFromIndex( ipm->mapToSource( selection.first() ) );

    QStandardItem *item = MainWindow::newItem(false, "");
    expand(selection.first());
    parent->appendRow(item);
    edit( ipm->mapFromSource( m->indexFromItem(item) ) );
}

void ListView::insertSubItemLink()
{
    ItemProxyModel * ipm = qobject_cast<ItemProxyModel *>(model());
    QStandardItemModel * m = qobject_cast<QStandardItemModel *>(ipm->sourceModel());
    QModelIndexList selection = selectedIndexes();
    if( selection.isEmpty() ) { return; }
    QStandardItem *parent = m->itemFromIndex( ipm->mapToSource( selection.first() ) );

    LinkEditDialog dlg;
    if( dlg.exec() == QDialog::Accepted )
    {
        QStandardItem *item = MainWindow::newItem(false, dlg.label(), QDateTime(), dlg.url());
        expand(selection.first());
        parent->appendRow(item);
    }
}

void ListView::insertHyperlink()
{
    QStandardItem *item = getCurrentItem();
    if( item != nullptr )
    {
        LinkEditDialog dlg( item->data(MainWindow::Label).toString(), item->data(MainWindow::Url).toString() );
        if( dlg.exec() == QDialog::Accepted )
        {
            item->setText( dlg.label() );
            item->setData( dlg.url() , MainWindow::Url );
        }
    }
}

void ListView::removeHyperlink()
{
    QStandardItem *item = getCurrentItem();
    if( item != nullptr )
    {
        item->setData( QUrl() , MainWindow::Url );
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
