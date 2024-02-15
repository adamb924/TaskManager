#include "event.h"

#include <QtDebug>

Event::Event() : mCompleted(false)
{

}

Event::Event(const QString &label, const QDateTime &dateTime) : mLabel(label), mDateTime(dateTime), mCompleted(false)
{
}

Event::Event(const Event &other) : mLabel(other.mLabel), mDateTime(other.mDateTime), mCompleted(other.mCompleted)
{

}

bool Event::operator==(const Event &other) const
{
    return mLabel == other.mLabel && mDateTime == other.mDateTime && mCompleted == other.mCompleted;
}

bool Event::operator<(const Event &other) const
{
    return mDateTime < other.mDateTime;
}

bool Event::operator>=(const Event &other) const
{
    return mDateTime >= other.mDateTime;
}

bool Event::compare(const Event &a, const Event &b)
{
    if( a.dateTime() < b.dateTime() )
        return true;
    else if( a.dateTime() == b.dateTime() && a.label() < b.label() )
        return true;
    return false;
}

bool Event::pointerComparison(const Event *a, const Event *b)
{
    if( a->dateTime() < b->dateTime() )
        return true;
    else if( a->dateTime() == b->dateTime() && a->label() < b->label() )
        return true;
    return false;
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
