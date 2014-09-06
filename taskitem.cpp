#include "taskitem.h"

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QtDebug>

TaskItem::TaskItem(const QString &label, const QDateTime &dateTime) :
    mLabel(label), mDate(dateTime)
{
    setSelectable(true);
    setEditable(true);
    setCheckable(true);
    setDragEnabled(true);
    setDropEnabled(true);
    setCheckState( mDate.isValid() ? Qt::Checked : Qt::Unchecked );

    setText("Hello world");
}

QVariant TaskItem::data(int role) const
{
    if( role == Qt::EditRole )
    {
        return mLabel;
    }
    else if ( role == Qt::DisplayRole )
    {
        if( mDate.isValid() )
        {
            return QObject::tr("%1 (%2)").arg(mLabel).arg( mDate.toString( Qt::TextDate ) );
        }
        else
        {
            return mLabel;
        }
    }
    return QStandardItem::data(role);
}

void TaskItem::setData(const QVariant &value, int role)
{
    QStandardItem::setData(value, role);
    if( role == Qt::EditRole )
    {
        mLabel = value.toString();
        emit emitDataChanged();
    }
    else if( role == Qt::CheckStateRole )
    {
        qDebug() << "check";
        if( checkState() == Qt::Checked )
        {
            mDate = QDateTime::currentDateTime();
        }
        else
        {
            mDate = QDateTime();
        }
        emit emitDataChanged();
    }
}

int TaskItem::type() const
{
    return QStandardItem::UserType+1;
}

TaskItem *TaskItem::clone()
{
    return new TaskItem(mLabel, mDate);
}
/*
void TaskItem::read(QDataStream &in)
{
    setSelectable(true);
    setEditable(true);
    setCheckable(true);
    setDragEnabled(true);
    setDropEnabled(true);
    setCheckState( mDate.isValid() ? Qt::Checked : Qt::Unchecked );
    mLabel = "label";

    setText("It's read");
}

void TaskItem::write(QDataStream &out) const
{

}
*/
