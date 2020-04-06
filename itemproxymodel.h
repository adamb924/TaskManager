#ifndef ITEMPROXYMODEL_H
#define ITEMPROXYMODEL_H

#include <QSortFilterProxyModel>

class QStandardItem;

class ItemProxyModel : public  QSortFilterProxyModel
{
    Q_OBJECT

public:
    ItemProxyModel();

    QVariant data(const QModelIndex &index, int role) const;
    void setDateTimeFormat(const QString & date);

private:
    QBrush getItemColor(const QStandardItem *item) const;

    QString mDateFormat;
};

#endif // ITEMPROXYMODEL_H
