#include "macrotask.h"

#include <QSize>
#include <QPainter>
#include <QPalette>

int Macrotask::MACROTASK_SIZE = 200;

Macrotask::Macrotask()
{

}

Macrotask::Macrotask(const QString &header, const QString &description) : mHeader(header), mDescription(description)
{

}

Macrotask::Macrotask(const Macrotask &other) : mHeader(other.header()), mDescription(other.description())
{

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

    painter->setPen(palette.windowText().color());
    painter->setBrush(palette.windowText());

    const int titleHeight = painter->fontMetrics().lineSpacing();
    const QRect textRect = rect.adjusted(3,titleHeight,3,3);
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