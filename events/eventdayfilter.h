#ifndef EVENTDAYFILTER_H
#define EVENTDAYFILTER_H

#include <QSortFilterProxyModel>
#include <QDate>

class EventDayFilter : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    EventDayFilter( const QDate & startDate, const QDate & endDate, bool displayIncomplete, QObject *parent = nullptr );

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QDate mStartDate;
    QDate mEndDate;
    bool mDisplayIncomplete;
};

#endif // EVENTDAYFILTER_H
