#ifndef ITEMPROXYMODEL_H
#define ITEMPROXYMODEL_H

#include <QSortFilterProxyModel>

class ItemProxyModel : public  QSortFilterProxyModel
{
    Q_OBJECT

public:
    ItemProxyModel();

    QVariant data(const QModelIndex &index, int role) const;
    void setDateTimeFormat(const QString & date);

private:
    QString mDateFormat;
};

#endif // ITEMPROXYMODEL_H
