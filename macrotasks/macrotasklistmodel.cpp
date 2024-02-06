#include "macrotasklistmodel.h"

#include <QMimeData>
#include <QDataStream>
#include <QtDebug>

QString MacrotaskListModel::MACROTASK_MIME_TYPE = "application/taskmanager.macrotask";

MacrotaskListModel::MacrotaskListModel(QList<Macrotask> *macrotaskList, QObject *parent)
    : QAbstractListModel(parent),
    mMacrotaskList(macrotaskList)
{
}

int MacrotaskListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return mMacrotaskList->count();
}

QVariant MacrotaskListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if ( role == Qt::DisplayRole)
    {
        return QVariant::fromValue( mMacrotaskList->at(index.row()) );
    }

    return QVariant();
}

bool MacrotaskListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    qDebug() << "MacrotaskListModel::insertRows" << row << count << parent;
    beginInsertRows(parent, row, row+count);
    for(int i=0; i<count; i++)
        mMacrotaskList->insert(row, Macrotask());
    endInsertRows();
    return count > 0;
}

bool MacrotaskListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    qDebug() << "MacrotaskListModel::removeRows" << row << count << parent;
    beginRemoveRows(parent, row, row+count);
    for(int i=0; i<count; i++)
        mMacrotaskList->removeAt(row);
    endRemoveRows();
    return count > 0;
}

Qt::DropActions MacrotaskListModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction | Qt::TargetMoveAction;
}


bool MacrotaskListModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    Q_UNUSED(sourceParent) /// invalid
    Q_UNUSED(destinationParent) /// invalid
    Q_UNUSED(count) /// must be 1

    emit layoutAboutToBeChanged();

    Macrotask t = mMacrotaskList->takeAt(sourceRow);
    if( sourceRow < destinationChild )
        destinationChild--;
    mMacrotaskList->insert(destinationChild, t);
    emit layoutChanged();
    return true;
}

bool MacrotaskListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        mMacrotaskList->replace( index.row(), qvariant_cast<Macrotask>(value) );
        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

Qt::ItemFlags MacrotaskListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return QAbstractItemModel::flags(index) | Qt::ItemIsDropEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsDragEnabled;
}
