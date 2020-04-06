#include "eventitemmodel.h"

#include <QModelIndex>
#include <QDateTime>
#include <QtDebug>
#include <QFont>

#include "event.h"

EventItemModel::EventItemModel(QList<Event *> * events, QObject *parent)
    : QAbstractItemModel(parent),
      mEvents(events)
{
    updateQuickAccessData();
}

void EventItemModel::addEvent(Event *e)
{
    mEvents->append(e);
    updateQuickAccessData();
    emit dataChanged( index(0,0,QModelIndex()), index(rowCount(), 1, QModelIndex()) );
}

QVariant EventItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section)
    Q_UNUSED(orientation)
    Q_UNUSED(role)
    return QVariant();
}

QModelIndex EventItemModel::index(int row, int column, const QModelIndex &parent) const
{
    if( parent == QModelIndex() ) // then it's a date
    {
        if( column == 0 )
            return createIndex(row,column,nullptr);
        else
            return QModelIndex();
    }
    else // it's an event
    {
        QDate d = mDates.at( parent.row() );
        return createIndex( row, column, mData.value(d).at(row) );
    }
}

QModelIndex EventItemModel::parent(const QModelIndex &index) const
{
    if( index.internalPointer() == nullptr ) // top-level
    {
        return QModelIndex();
    }
    else
    {
        Event * e = static_cast<Event*>( index.internalPointer() );
        return createIndex( mDates.indexOf( e->date() ), 0, nullptr );
    }
}

int EventItemModel::rowCount(const QModelIndex &parent) const
{
    if( parent == QModelIndex() ) /// top-level
    {
        return mDates.count();
    }
    else if ( parent.parent() == QModelIndex() )
    {
        return mData.value( mDates.at( parent.row() ) ).count();
    }
    else
    {
        return 0;
    }
}

int EventItemModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 2;
}

QVariant EventItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if( role == Qt::DisplayRole || role == Qt::EditRole )
    {
        if( index.parent() == QModelIndex() ) /// top-level
        {
            if( index.column() == 0 )
                return mDates.at( index.row() ).toString("dddd, MMMM d, yyyy");
        }
        else /// event
        {
            switch( index.column() )
            {
            case 0:
                return mData.value( mDates.at( index.parent().row() ) ).at( index.row() )->time().toString("h:mmap");
            case 1:
                return mData.value( mDates.at( index.parent().row() ) ).at( index.row() )->label();
            default:
                return QVariant();
            }
        }
    }
    else if ( role == Qt::CheckStateRole )
    {
        if( index.parent() != QModelIndex() ) /// non-top-level
        {
            if( index.column() == 0 )
            {
                return mData.value( mDates.at( index.parent().row() ) ).at( index.row() )->completed() ? Qt::Checked : Qt::Unchecked;
            }
        }
    }
    else if ( role == EventItemModel::Date )
    {
        if( index.parent() == QModelIndex() ) /// top-level (a date)
        {
            return mDates.at(index.row());
        }
        else
        {
            Event * e = static_cast<Event*>( index.internalPointer() );
            return e->date();
        }
    }
    else if ( role == EventItemModel::Type )
    {
        if( index.parent() == QModelIndex() ) /// top-level (a date)
        {
            return EventItemModel::DateType;
        }
        else
        {
            return EventItemModel::EventType;
        }
    }
    else if ( role == Qt::FontRole )
    {
        if( index.parent() == QModelIndex() ) /// it's a top-level item (a date)
        {
            if( mDates.at(index.row()) == QDateTime::currentDateTime().date() )
            {
                QFont f;
                f.setBold(true);
                return f;
            }
        }
    }

    return QVariant();
}

Qt::ItemFlags EventItemModel::flags(const QModelIndex &index) const
{
    if( index.parent() == QModelIndex() )
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    else
    {
        switch(index.column())
        {
        case 0:
            return Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
        case 1:
        default:
            return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
        }
    }
}

bool EventItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if( index.parent() == QModelIndex() ) /// dates are just not editable
    {
        return false;
    }
    else
    {
        Event * e = static_cast<Event*>( index.internalPointer() );
        if( index.column() == 0 ) /// time
        {
            if( role == Qt::CheckStateRole )
            {
                e->setCompleted( value.toInt() == Qt::Checked );
                emit dataChanged(index, index, QVector<int>() << role );
                return true;
            }
        }
        else if ( index.column() == 1 ) /// label
        {
            if( role == Qt::EditRole )
            {
                e->setLabel( value.toString() );
                emit dataChanged(index, index, QVector<int>() << role );
                return true;
            }
        }
    }
    return false;
}

bool EventItemModel::removeRow(int row, const QModelIndex &parent)
{
    if( parent == QModelIndex() ) /// if it's a date
    {
        return false;
    }
    beginRemoveRows(parent, row, row);
    QDate d = mDates.at( parent.row() );
    Event *e = mData.value(d).at(row);
    mEvents->removeOne( e );
    updateQuickAccessData();
    endRemoveRows();
    return true;
}

void EventItemModel::updateQuickAccessData()
{
    QDate today = QDateTime::currentDateTime().date();

    QMultiHash<QDate, Event*> hash;
    foreach(Event * e, *mEvents)
    {
        if( ! e->completed() || e->date() >= today )
            hash.insertMulti( e->date(), e);
    }

    mDates = hash.uniqueKeys();
    std::sort( mDates.begin(), mDates.end()  );

    foreach(QDate d, mDates)
    {
        QList<Event*> list = hash.values( d );
        std::sort( list.rbegin(), list.rend()  );

        qDebug() << list;
        foreach( Event * e, list ) {
            qDebug() << *e;
        }

        mData[ d ] = list;
    }

//    qDebug() << mDates;
//    qDebug() << mData;
}
