#include "eventdayfilter.h"

#include "eventitemmodel.h"

#include <QtDebug>

EventDayFilter::EventDayFilter(const QDate &startDate, const QDate &endDate, bool displayIncomplete, QObject *parent)
    : QSortFilterProxyModel(parent),
        mStartDate(startDate),
        mEndDate(endDate),
        mDisplayIncomplete(displayIncomplete)
{
    setDynamicSortFilter(true);
}

bool EventDayFilter::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QDate date = sourceModel()->data( sourceModel()->index(sourceRow,0, sourceParent), EventItemModel::Date).toDate();
    bool unchecked = sourceModel()->data( sourceModel()->index(sourceRow,0, sourceParent), Qt::CheckStateRole).toInt() == Qt::Unchecked;
    bool before = mEndDate.isNull() || date <= mEndDate;
    bool between = date >= mStartDate && ( mEndDate.isNull() || date <= mEndDate );
    return between || ( unchecked && before && mDisplayIncomplete);
}
