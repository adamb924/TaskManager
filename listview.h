#ifndef LISTVIEW_H
#define LISTVIEW_H

#include <QTreeView>

class QStandardItemModel;

class ListView : public QTreeView
{
    Q_OBJECT

public:
    explicit ListView(QWidget *parent = 0);

    void setArchive(QStandardItemModel * archive);
    void setDateTimeFormat(const QString & date);

signals:
    void showArchive();
    void preferences();
    void save();

protected slots:
    void insert();
    void insertSubItem();
    void remove();
    void archive();

protected:
    bool isArchive;
    void contextMenuEvent(QContextMenuEvent *e);

private:
    QStandardItemModel * mArchive;
    QString mDateFormat;
};

#endif // LISTVIEW_H
