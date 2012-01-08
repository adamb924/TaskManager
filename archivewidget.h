#ifndef ARCHIVEWIDGET_H
#define ARCHIVEWIDGET_H

#include "listwidget.h"

class ArchiveWidget : public ListWidget
{
    Q_OBJECT
public:
    explicit ArchiveWidget(QString init, QString date, QString time, QWidget *parent = 0);

    void setDateFormat(QString format) { dateFormat = format; }
    void setTimeFormat(QString format) { timeFormat = format; }

protected:
    void contextMenuEvent(QContextMenuEvent *e);
    void dropEvent(QDropEvent *event);

    QString dateFormat, timeFormat;

signals:

public slots:
    void addToArchive(QString text, Qt::CheckState state);

};

#endif // ARCHIVEWIDGET_H
