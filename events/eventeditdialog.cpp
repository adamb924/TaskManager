#include "eventeditdialog.h"
#include "ui_eventeditdialog.h"

#include <QtDebug>

#include "event.h"

EventEditDialog::EventEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EventEditDialog)
{
    ui->setupUi(this);

    ui->dateTimeEdit->setDate( QDate::currentDate() );
    ui->dateTimeEdit->setTime( QTime::fromString("12:00", "hh:mm") );

    connect(this, SIGNAL(accepted()), this, SLOT(setEventData()));
}

EventEditDialog::EventEditDialog(Event event, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EventEditDialog),
    mEvent(event)
{
    ui->setupUi(this);

    ui->lineEdit->setText( mEvent.label() );
    ui->dateTimeEdit->setDate( mEvent.date() );
    ui->dateTimeEdit->setTime( mEvent.time() );

    connect(this, SIGNAL(accepted()), this, SLOT(setEventData()));
}

EventEditDialog::~EventEditDialog()
{
    delete ui;
}

Event EventEditDialog::getEvent()
{
    return mEvent;
}

void EventEditDialog::setDate(const QDate &date)
{
    if( date.isValid() )
        ui->dateTimeEdit->setDate( date );
}

void EventEditDialog::setEventData()
{
    mEvent.setLabel( ui->lineEdit->text() );
    mEvent.setDateTime( ui->dateTimeEdit->dateTime() );
}
