#include "event.h"

#include <QtDebug>

Event::Event() : mCompleted(false)
{

}

Event::Event(const QString &label, const QDateTime &dateTime) : mLabel(label), mDateTime(dateTime), mCompleted(false)
{
}

bool Event::operator<(const Event &other) const
{
    return mDateTime < other.mDateTime;
}

bool Event::operator>=(const Event &other) const
{
    return mDateTime >= other.mDateTime;
}

bool Event::pointerComparison(const Event *a, const Event *b)
{
    return *a < *b;
}

QString Event::label() const
{
    return mLabel;
}

void Event::setLabel(const QString &label)
{
    mLabel = label;
}

QDate Event::date() const
{
    return mDateTime.date();
}

QTime Event::time() const
{
    return mDateTime.time();
}

QDateTime Event::dateTime() const
{
    return mDateTime;
}

void Event::setDateTime(const QDateTime &dateTime)
{
    mDateTime = dateTime;
}

bool Event::completed() const
{
    return mCompleted;
}

void Event::setCompleted(bool completed)
{
    mCompleted = completed;
}

QDebug operator<<(QDebug dbg, const Event &key)
{
    dbg.nospace() << "EventBit(Label: " << key.label() << ", DateTime: " << key.dateTime() << ", Completed: " << key.completed() << ")";
    return dbg.maybeSpace();
}
