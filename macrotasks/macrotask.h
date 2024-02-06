#ifndef MACROTASK_H
#define MACROTASK_H

#include <QString>
#include <QMetaType>

class QPainter;
class QStyleOptionViewItem;
class QModelIndex;
class QSize;

class Macrotask
{
public:
    Macrotask();
    Macrotask(const QString & header, const QString & description);
    Macrotask(const Macrotask & other);

    friend QDataStream & operator<<(QDataStream &arch, const Macrotask & object);
    friend QDataStream & operator>>(QDataStream &arch, Macrotask & object);

    QString header() const;
    void setHeader(const QString &newHeader);

    QString description() const;
    void setDescription(const QString &newDescription);

    void paint(QPainter *painter, const QRect &rect, const QPalette &palette) const;
    QSize sizeHint() const;

    static int MACROTASK_SIZE;

private:
    QString mHeader;
    QString mDescription;
};

Q_DECLARE_METATYPE(Macrotask)

QDebug operator<<(QDebug dbg, const Macrotask &key);

#endif // MACROTASK_H
