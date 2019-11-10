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
#include <QClipboard>

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
        QStandardItem * item = getItem(e->pos());

        menu.addAction( tr("Delete this item"), this,SLOT(remove()) );

        QAction *putOnHold = new QAction(tr("Put on hold"));
        putOnHold->setCheckable(true);
        putOnHold->setChecked( item->data(MainWindow::PutOnHold).toBool() );
        connect( putOnHold, SIGNAL(triggered()), this, SLOT(togglePutOnHold()) );
        menu.addAction( putOnHold );


        menu.addAction(tr("Archive this item"), this,SLOT(archive()));
        menu.addAction(tr("Insert subitem"), this,SLOT(insertSubItem()) );
        menu.addSeparator();
        if( item->data(MainWindow::Url).toUrl().isValid() )
        {
            menu.addAction(tr("Edit link"), this,SLOT(insertHyperlink()) );
            menu.addAction(tr("Remove link"), this,SLOT(removeHyperlink()) );
        }
        else
        {
            menu.addAction(tr("Add link"), this,SLOT(insertHyperlink()) );
        }
        menu.addAction(tr("Insert link as subitem"), this,SLOT(insertSubItemLink()) );
    }
    else
    {
        menu.addAction(tr("Insert an item"), this,SLOT(insert()) );
        menu.addAction(tr("Insert a link"), this,SLOT(insertLink()) );
    }
    menu.addSeparator();

    menu.addAction(tr("Copy text"), this,SLOT(copyItem()) );

    menu.addSeparator();

    QMenu * applicationMenu = new QMenu(tr("Application"), this);
    applicationMenu->addAction(tr("Show the archive"), this,SIGNAL(showArchive()) );
    applicationMenu->addAction(tr("Preferences"), this,SIGNAL(preferences()) );

    applicationMenu->addSeparator();
    applicationMenu->addAction(tr("Open..."), this, SIGNAL(openFile()));
    applicationMenu->addAction(tr("Open the data directory..."), this, SIGNAL(openDataDirectory()));
    applicationMenu->addSeparator();
    applicationMenu->addAction(tr("Save"), this,SIGNAL(save()) );
    applicationMenu->addAction(tr("Save as..."), this, SIGNAL(saveAs()));

    menu.addMenu(applicationMenu);

    menu.exec(e->globalPos());
}

QStandardItem *ListView::getSelectedItem()
{
    ItemProxyModel * ipm = qobject_cast<ItemProxyModel *>(model());
    QStandardItemModel * m = qobject_cast<QStandardItemModel *>(ipm->sourceModel());
    QModelIndexList selection = selectedIndexes();
    if( selection.isEmpty() ) { return nullptr; }
    return m->itemFromIndex( ipm->mapToSource( selection.first() ) );
}

QStandardItem *ListView::getItem(const QPoint &pos)
{
    ItemProxyModel * ipm = qobject_cast<ItemProxyModel *>(model());
    QStandardItemModel * m = qobject_cast<QStandardItemModel *>(ipm->sourceModel());
    return m->itemFromIndex( ipm->mapToSource( indexAt( pos ) ) );
}

void ListView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if( QGuiApplication::keyboardModifiers() == Qt::ControlModifier )
    {
        openLink(event->pos());
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

    if( selection.first().parent() != QModelIndex() )
    {
        QStandardItem *parent = m->itemFromIndex( selected.parent() );
        parent->takeRow( selected.row() );
    }
    else
    {
        m->takeRow( selected.row() );
    }
    emit archiveItem(item, m->objectName());
}

void ListView::copyItem()
{
    QStandardItem *item = getSelectedItem();
    if( item != nullptr )
    {
        QGuiApplication::clipboard()->setText( getItemText(item) );
    }
}

QString ListView::getItemText(QStandardItem * item, const QString & subitemPrefix)
{
    if( item->rowCount() == 0 )
    {
        return item->text();
    }
    else
    {
        QString text = item->text();
        for(int i=0; i<item->rowCount(); i++)
        {
            text += "\r\n" + subitemPrefix + "\t" + getItemText( item->child(i), tr("\t") + subitemPrefix);
        }
        return text;
    }
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

void ListView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        QStandardItem * item = getItem(event->pos());
        if( item != nullptr )
        {
            item->setData(model()->objectName(), MainWindow::OriginList);
        }
    }
    if( QGuiApplication::keyboardModifiers() == Qt::ControlModifier && event->button() == Qt::LeftButton )
    {
        openLink(event->pos());
    }
    else
    {
        QTreeView::mousePressEvent(event);
    }
}

void ListView::openLink(const QPoint &pos)
{
    QStandardItem * item = getItem(pos);
    if( item != nullptr )
    {
        QUrl url = item->data(MainWindow::Url).toUrl();
        if( !url.isEmpty() )
        {
            QDesktopServices::openUrl( url );
        }
    }
}

void ListView::togglePutOnHold()
{
    QStandardItem *item = getSelectedItem();
    if( item != nullptr )
    {
        item->setData( ! item->data(MainWindow::PutOnHold).toBool(), MainWindow::PutOnHold );
    }
}

void ListView::insert(QUrl url)
{
    ItemProxyModel * ipm = qobject_cast<ItemProxyModel *>(model());
    QStandardItemModel * m = qobject_cast<QStandardItemModel *>(ipm->sourceModel());
    QStandardItem *item = MainWindow::newItem(false, "", QDateTime::currentDateTime());
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
        QStandardItem *item = MainWindow::newItem(false, dlg.label(), QDateTime::currentDateTime(), QDateTime(), dlg.url());
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

    QStandardItem *item = MainWindow::newItem(false, "", QDateTime::currentDateTime());
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
        QStandardItem *item = MainWindow::newItem(false, dlg.label(), QDateTime::currentDateTime(), QDateTime(), dlg.url());
        expand(selection.first());
        parent->appendRow(item);
    }
}

void ListView::insertHyperlink()
{
    QStandardItem *item = getSelectedItem();
    if( item != nullptr )
    {
        LinkEditDialog dlg( item->data(MainWindow::Label).toString(), item->data(MainWindow::Url).toString(), true );
        if( dlg.exec() == QDialog::Accepted )
        {
            item->setText( dlg.label() );
            item->setData( dlg.url() , MainWindow::Url );
        }
    }
}

void ListView::removeHyperlink()
{
    QStandardItem *item = getSelectedItem();
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
