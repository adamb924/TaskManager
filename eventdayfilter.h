#ifndef EVENTDAYFILTER_H
#define EVENTDAYFILTER_H

#include <QSortFilterProxyModel>
#include <QDate>

class EventDayFilter : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    EventDayFilter( const QDate & startDate, const QDate & endDate, QObject *parent = nullptr );

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QDate mStartDate;
    QDate mEndDate;
};

#endif // EVENTDAYFILTER_H
