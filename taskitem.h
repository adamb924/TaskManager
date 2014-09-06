#ifndef TASKITEM_H
#define TASKITEM_H

#include <QStandardItem>

#include <QDateTime>

class TaskItem : public QStandardItem
{
public:
    TaskItem(const QString & label, const QDateTime & dateTime = QDateTime() );

    QVariant data(int role = Qt::UserRole + 1) const;
    void setData(const QVariant & value, int role = Qt::EditRole);
    int type() const;
    TaskItem * clone();
//    void read(QDataStream &in);
//    void write(QDataStream &out) const;

private:
    QString mLabel;
    QDateTime mDate;
};

#endif // TASKITEM_H
