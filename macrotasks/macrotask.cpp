#include "macrotask.h"

#include <QtDebug>
#include <QSize>
#include <QPainter>
#include <QPalette>

int Macrotask::MACROTASK_SIZE = 200;

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

void Macrotask::paint(QPainter *painter, const QRect &rect, const QPalette &palette) const
{
    painter->save();

    painter->fillRect(rect, mColor);

    painter->setPen(palette.windowText().color());
    painter->setBrush(palette.windowText());

    const int titleHeight = painter->fontMetrics().lineSpacing();
    const QRect textRect = rect.adjusted(3,titleHeight,-3,3);
    QFont normal = painter->font();
    QFont bold = painter->font();
    bold.setBold(true);

    painter->setFont( bold );
    painter->drawText( textRect.topLeft(), mHeader );
    painter->setFont( normal );
    painter->drawText( textRect.translated(0,titleHeight), Qt::TextWordWrap, mDescription );

    painter->restore();
}

QSize Macrotask::sizeHint() const
{
    return QSize(MACROTASK_SIZE,MACROTASK_SIZE);
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
