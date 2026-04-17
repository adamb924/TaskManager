#include "macrotaskitemdelegate.h"

#include <QPainter>
#include "macrotask.h"
#include "macrotasklistmodel.h"

int MacrotaskItemDelegate::MACROTASK_SIZE = 200;

MacrotaskItemDelegate::MacrotaskItemDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{

}

void MacrotaskItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.data().canConvert<Macrotask>())
    {
        painter->save();

        painter->fillRect(option.rect, index.data(Qt::BackgroundRole).value<QBrush>());

        painter->setPen(option.palette.windowText().color());
        painter->setBrush(option.palette.windowText());

        const int titleHeight = painter->fontMetrics().lineSpacing();
        const QRect textRect = option.rect.adjusted(3, titleHeight, -3, 3);
        QFont normal = painter->font();
        QFont bold = painter->font();
        bold.setBold(true);

        painter->setFont(bold);
        painter->drawText(textRect.topLeft(),
                          index.data(MacrotaskListModel::Header).value<QString>());
        painter->setFont(normal);
        painter->drawText(textRect.translated(0, titleHeight),
                          Qt::TextWordWrap,
                          index.data(MacrotaskListModel::Description).value<QString>());

        painter->restore();
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize MacrotaskItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.data().canConvert<Macrotask>())
    {
        return QSize(MACROTASK_SIZE, MACROTASK_SIZE);
    }
    else
    {
        return QStyledItemDelegate::sizeHint(option, index);
    }
}
