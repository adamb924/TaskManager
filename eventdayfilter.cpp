#include "eventdayfilter.h"

#include "eventitemmodel.h"

#include <QtDebug>

EventDayFilter::EventDayFilter(const QDate &startDate, const QDate &endDate, QObject *parent)
    : QSortFilterProxyModel(parent),
        mStartDate(startDate),
        mEndDate(endDate)
{
    setDynamicSortFilter(true);
}

bool EventDayFilter::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QDate date = sourceModel()->data( sourceModel()->index(sourceRow,0, sourceParent), EventItemModel::Date).toDate();
    bool between = date >= mStartDate && ( mEndDate.isNull() || date <= mEndDate );
    return between;
}
