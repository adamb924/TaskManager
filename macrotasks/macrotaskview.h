#ifndef MACROTASKVIEW_H
#define MACROTASKVIEW_H

#include <QListView>

class MacrotaskView : public QListView
{
    Q_OBJECT
public:
    explicit MacrotaskView(QWidget *parent = nullptr);

protected:
    void contextMenuEvent(QContextMenuEvent *e) override;
    void addMacrotask(int where);
    void editMacrotask(int which);
    void removeMacrotask(int which);

};

#endif // MACROTASKVIEW_H
