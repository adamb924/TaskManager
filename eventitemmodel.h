#ifndef EVENTITEMMODEL_H
#define EVENTITEMMODEL_H

#include <QAbstractItemModel>
#include <QDate>

class Event;

class EventItemModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    EventItemModel(QList<Event *> *events, QObject *parent = nullptr);

    enum ItemRole { Date = Qt::UserRole + 1, Type };
    enum ItemType { NoType, DateType, EventType };

    void addEvent(Event * e);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    bool removeRow(int row, const QModelIndex &parent = QModelIndex());

private:
    void updateQuickAccessData();

    QList<Event*> * mEvents;

    QList<QDate> mDates;
    QHash<QDate, QList<Event*>> mData;
};

#endif // EVENTITEMMODEL_H
