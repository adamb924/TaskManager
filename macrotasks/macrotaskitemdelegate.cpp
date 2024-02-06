#include "macrotaskitemdelegate.h"

#include "macrotask.h"
#include <QPainter>

MacrotaskItemDelegate::MacrotaskItemDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{

}

void MacrotaskItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.data().canConvert<Macrotask>())
    {
        if (option.state & QStyle::State_Selected)
        {
//            painter->setRenderHint(QPainter::Antialiasing, true);
//            painter->setPen( option.palette.highlight().color() );
//            painter->drawRoundedRect(option.rect.adjusted(0,0,-1,-1), 3, 3);
            painter->fillRect(option.rect, option.palette.highlight());
        }

        Macrotask task = qvariant_cast<Macrotask>(index.data());
        task.paint(painter, option.rect, option.palette);
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
        Macrotask task = qvariant_cast<Macrotask>(index.data());
        return task.sizeHint();
    }
    else
    {
        return QStyledItemDelegate::sizeHint(option, index);
    }
}
