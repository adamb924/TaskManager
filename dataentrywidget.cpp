#include "dataentrywidget.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

#include <QtDebug>

DataEntryWidget::DataEntryWidget(QString ul, QString ur, QString ll, QString lr, QString dateFormat, QString timeFormat, QWidget *parent = 0) :
    QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;

//    QLineEdit *ul, *ur, *ll, *lr, *dateFormat;
    this->ul = new QLineEdit(ul);
    this->ur = new QLineEdit(ur);
    this->ll = new QLineEdit(ll);
    this->lr = new QLineEdit(lr);
    this->dateFormat = new QLineEdit(dateFormat);
    this->timeFormat = new QLineEdit(timeFormat);

    layout->addWidget(new QLabel(tr("Upper left label")));
    layout->addWidget(this->ul);
    layout->addWidget(new QLabel(tr("Upper right label")));
    layout->addWidget(this->ur);
    layout->addWidget(new QLabel(tr("Lower left label")));
    layout->addWidget(this->ll);
    layout->addWidget(new QLabel(tr("Lower right label")));
    layout->addWidget(this->lr);

    layout->addWidget(new QLabel(tr("Date format (e.g. ddd MMMM d yy becomes Sun July 20 69")));
    layout->addWidget(this->dateFormat);
    layout->addWidget(new QLabel(tr("Time format (e.g. hh:mm:ss.zzz becomes 14:13:09.042")));
    layout->addWidget(this->timeFormat);

    QPushButton *accept = new QPushButton(tr("Accept"));
    layout->addWidget(accept);

    connect(accept,SIGNAL(clicked()),this,SLOT(accept()));

    this->setLayout(layout);
}
