#ifndef EVENT_H
#define EVENT_H

#include <QString>
#include <QDateTime>

class Event
{
public:
    Event();
    Event(const QString & label, const QDateTime & dateTime);

    bool operator<(const Event & other) const;
    bool operator>=(const Event & other) const;
    static bool pointerComparison(const Event * a, const Event * b);

    QString label() const;
    void setLabel(const QString &label);

    QDate date() const;
    QTime time() const;
    QDateTime dateTime() const;
    void setDateTime(const QDateTime &dateTime);

    bool completed() const;
    void setCompleted(bool completed);

private:
    QString mLabel;
    QDateTime mDateTime;
    bool mCompleted;
};

QDebug operator<<(QDebug dbg, const Event &key);

#endif // EVENT_H
