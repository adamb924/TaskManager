#ifndef EVENTVIEW_H
#define EVENTVIEW_H

#include <QTreeView>

class EventItemModel;

class EventView : public QTreeView
{
    Q_OBJECT
public:
    EventView(QWidget *parent);

    void setEventModel(EventItemModel *m);

private slots:
    void addEvent();
    void editEvent();
    void removeEvent();
    void spanFirstColumns();

protected:
    void contextMenuEvent(QContextMenuEvent *e);

private:
    EventItemModel * mModel;
};

#endif // EVENTVIEW_H
