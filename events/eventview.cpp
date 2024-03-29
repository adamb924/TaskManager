#include "eventview.h"

#include <QMenu>
#include <QContextMenuEvent>
#include <QtDebug>

#include "event.h"
#include "eventeditdialog.h"
#include "eventitemmodel.h"
#include "eventdayfilter.h"

EventView::EventView(QWidget *parent) : QTreeView(parent)
{
    setHeaderHidden(true);
}

void EventView::addEvent()
{
    EventEditDialog dlg(this);

    /// autofill the date for the user
    QModelIndexList selection = selectionModel()->selectedIndexes();
    if( selection.count() > 0 )
    {
        dlg.setDate( model()->data( selection.first() , EventItemModel::Date ).toDate() );
    }

    if( dlg.exec() == QDialog::Accepted )
    {
        EventDayFilter * proxy = qobject_cast<EventDayFilter*>(model());
        if( proxy != nullptr )
        {
            EventItemModel * model = qobject_cast<EventItemModel*>(proxy->sourceModel());
            if( model != nullptr )
            {
                model->addEvent( new Event( dlg.getEvent() ) );
            }
        }

    }
}

void EventView::editEvent()
{
    /// autofill the date for the user
    QModelIndexList selection = selectionModel()->selectedIndexes();
    if( selection.count() > 0 )
    {
        QModelIndex index = selection.first(); /// there ought only ever to be one anyway

        EventDayFilter * proxy = qobject_cast<EventDayFilter*>(model());
        if( proxy != nullptr )
        {
            QModelIndex remapped = proxy->mapToSource( index );
            Event * e = static_cast<Event*>(remapped.internalPointer());
            EventEditDialog dlg( *e, this);
            if( dlg.exec() )
            {
                /// TODO this is terrible; the event pointer is already updated,
                /// I just call the model's setData() method to call the
                /// necessary update functions
                *e = dlg.getEvent(); /// make sure all the data are copied
                model()->setData( index, dlg.getEvent().label() );
            }
        }
    }
}

void EventView::removeEvent()
{
    /// autofill the date for the user
    QModelIndexList selection = selectionModel()->selectedIndexes();
    if( selection.count() > 0 )
    {
        QModelIndex index = selection.first(); /// there ought only ever to be one anyway
        EventDayFilter * proxy = qobject_cast<EventDayFilter*>(model());
        if( proxy != nullptr )
        {
            EventItemModel * model = qobject_cast<EventItemModel*>(proxy->sourceModel());
            if( model != nullptr )
            {
                model->removeRow( proxy->mapToSource(index).row(), proxy->mapToSource(index).parent() );
            }
        }
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

void EventView::onModelReset()
{
    spanFirstColumns();
    expandAll();
}

void EventView::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu menu;

    QModelIndex index = indexAt(e->pos());
    EventItemModel::ItemType type = EventItemModel::NoType;
    if( index.isValid() )
        type = static_cast<EventItemModel::ItemType>(model()->data(index, EventItemModel::Type).toInt());

    menu.addAction(tr("Add event..."), this,SLOT(addEvent()) );
    if( type == EventItemModel::EventType )
    {
        menu.addAction(tr("Edit event..."), this,SLOT(editEvent()) );
        menu.addAction(tr("Remove event"), this,SLOT(removeEvent()) );
    }

    menu.exec(e->globalPos());
}
