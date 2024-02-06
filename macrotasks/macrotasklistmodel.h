#ifndef MACROTASKLISTMODEL_H
#define MACROTASKLISTMODEL_H

#include "macrotasks/macrotask.h"
#include <QAbstractListModel>

class MacrotaskListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ItemRole { Header = Qt::UserRole + 1, Description };

    explicit MacrotaskListModel(QList<Macrotask> * macrotaskList, QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
    QList<Macrotask> * mMacrotaskList;
};

#endif // MACROTASKLISTMODEL_H
