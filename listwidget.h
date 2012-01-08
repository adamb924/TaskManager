#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#include <QListWidget>
#include <QList>

class QContextMenuEvent;
class QStringList;
class QMimeData;
class QListWidgetItem;

class ListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit ListWidget(QString init, QWidget *parent = 0);

    QString serialize();

protected:
    bool isArchive;

    void contextMenuEvent(QContextMenuEvent *e);
    void dropEvent(QDropEvent *event);
    void dragMoveEvent ( QDragMoveEvent *event );
    QStringList mimeTypes() const;
    QMimeData* mimeData ( const QList<QListWidgetItem *> items ) const;

protected slots:
    void insert();
    void remove();
    void archive();

signals:
    void showArchive();
    void addToArchive(QString text, Qt::CheckState state);
    void preferences();
    void save();

public slots:

};

#endif // LISTWIDGET_H
