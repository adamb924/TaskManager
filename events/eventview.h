#ifndef EVENTVIEW_H
#define EVENTVIEW_H

#include <QTreeView>

class EventItemModel;
class EventDayFilter;
class Event;

class EventView : public QTreeView
{
    Q_OBJECT
public:
    EventView(QWidget *parent);

public slots:
    void spanFirstColumns();

signals:
    void addEvent(Event * e);
    void removeEvent(Event * e);
    void editEvent(Event * e);

private slots:
    void addEvent();
    void editEvent();
    void removeEvent();
    void onModelReset();

protected:
    void contextMenuEvent(QContextMenuEvent *e) override;
};

#endif // EVENTVIEW_H
