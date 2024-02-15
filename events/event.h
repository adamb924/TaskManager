#ifndef EVENT_H
#define EVENT_H

#include <QString>
#include <QDateTime>
#include <QMetaType>

class Event
{
public:
    Event();
    Event(const QString & label, const QDateTime & dateTime);
    Event(const Event & other);

    bool operator==(const Event & other) const;
    bool operator<(const Event & other) const;
    bool operator>=(const Event & other) const;
    static bool compare(const Event & a, const Event & b);
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

Q_DECLARE_METATYPE(Event)

QDebug operator<<(QDebug dbg, const Event &key);

#endif // EVENT_H
