#ifndef EVENTEDITDIALOG_H
#define EVENTEDITDIALOG_H

#include <QDialog>

namespace Ui {
class EventEditDialog;
}

#include "event.h"

class EventEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EventEditDialog(QWidget *parent = nullptr);
    EventEditDialog(Event event, QWidget *parent = nullptr);
    ~EventEditDialog();

    Event getEvent();

    void setDate(const QDate & date);


private slots:
    void setEventData();

private:
    Ui::EventEditDialog *ui;

    Event mEvent;
};

#endif // EVENTEDITDIALOG_H
