#ifndef MACROTASKITEMDELEGATE_H
#define MACROTASKITEMDELEGATE_H

#include <QStyledItemDelegate>

class MacrotaskItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit MacrotaskItemDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // MACROTASKITEMDELEGATE_H
