#include "eventview.h"

#include <QMenu>
#include <QContextMenuEvent>
#include <QtDebug>

#include "eventeditdialog.h"
#include "eventitemmodel.h"

EventView::EventView(QWidget *parent) : QTreeView(parent)
{
}

void EventView::addEvent()
{
    EventEditDialog dlg(this);

    /// autofill the date for the user
    QModelIndexList selection = selectionModel()->selectedIndexes();
    if( selection.count() > 0 )
    {
        dlg.setDate( mModel->data( selection.first() , EventItemModel::Date ).toDate() );
    }

    if( dlg.exec() == QDialog::Accepted )
    {
        mModel->addEvent( dlg.getEvent() );
    }
}

void EventView::editEvent()
{
    /// autofill the date for the user
    QModelIndexList selection = selectionModel()->selectedIndexes();
    if( selection.count() > 0 )
    {
        QModelIndex index = selection.first(); /// there ought only ever to be one anyway
        Event * e = static_cast<Event*>(index.internalPointer());

        EventEditDialog dlg(e, this);
        dlg.exec();
    }
}

void EventView::removeEvent()
{
    /// autofill the date for the user
    QModelIndexList selection = selectionModel()->selectedIndexes();
    if( selection.count() > 0 )
    {
        QModelIndex index = selection.first(); /// there ought only ever to be one anyway
        mModel->removeRow(index.row(), index.parent());
    }
}

void EventView::spanFirstColumns()
{
    int rowCount = model()->rowCount();
    for(int i=0; i<rowCount; i++)
    {
        setFirstColumnSpanned(i, QModelIndex(), true);
    }
    /// TODO bit of a hack; won't work for other fonts
    setColumnWidth(0, 115 );
}

void EventView::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu menu;

    QModelIndex index = indexAt(e->pos());
    EventItemModel::ItemType type = EventItemModel::NoType;
    if( index.isValid() )
        type = static_cast<EventItemModel::ItemType>(mModel->data(index, EventItemModel::Type).toInt());

    menu.addAction(tr("Add event..."), this,SLOT(addEvent()) );
    if( type == EventItemModel::EventType )
    {
        menu.addAction(tr("Edit event..."), this,SLOT(editEvent()) );
        menu.addAction(tr("Remove event"), this,SLOT(removeEvent()) );
    }

    menu.exec(e->globalPos());
}

void EventView::setEventModel(EventItemModel *m)
{
    mModel = m;
    QTreeView::setModel(m);
    connect( model(), SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(spanFirstColumns()));
    connect( model(), SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(expandAll()));

    /// get things started
    spanFirstColumns();
    expandAll();
}
