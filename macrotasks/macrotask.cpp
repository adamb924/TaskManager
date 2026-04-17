#include "macrotask.h"

#include <QtDebug>
#include <QSize>
#include <QPainter>
#include <QPalette>

Macrotask::Macrotask() : mColor(QColor(255,255,255))
{

}

Macrotask::Macrotask(const QString &header, const QString &description, const QColor &color)
    : mHeader(header),
    mDescription(description),
    mColor(color)
{

}

Macrotask::Macrotask(const Macrotask &other)
    : mHeader(other.mHeader),
    mDescription(other.mDescription),
    mColor(other.mColor)
{

}

Macrotask &Macrotask::operator=(const Macrotask &other)
{
    mHeader = other.mHeader;
    mDescription = other.mDescription;
    mColor = other.mColor;
    return *this;
}

QDataStream & operator << (QDataStream &arch, const Macrotask & object)
{
    arch << object.header();
    arch << object.description();
    arch << object.color().name();
    return arch;
}

QDataStream & operator >> (QDataStream &arch, Macrotask & object)
{
    QString str;
    arch >> str;
    object.setHeader(str);
    arch >> str;
    object.setDescription(str);
    arch >> str;
    object.setColor(QColor(str));
    return arch;
}

QString Macrotask::header() const
{
    return mHeader;
}

void Macrotask::setHeader(const QString &newHeader)
{
    mHeader = newHeader;
}

QString Macrotask::description() const
{
    return mDescription;
}

void Macrotask::setDescription(const QString &newDescription)
{
    mDescription = newDescription;
}

QColor Macrotask::color() const
{
    return mColor;
}

void Macrotask::setColor(const QColor &newColor)
{
    mColor = newColor;
}

QDebug operator<<(QDebug dbg, const Macrotask &key)
{
    dbg.nospace() << "Macrotask(Header: " << key.header() << ", Description: " << key.description() << ", Color: " << key.color().name() << ")";
    return dbg.maybeSpace();
}
