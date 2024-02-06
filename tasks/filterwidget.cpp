#include "filterwidget.h"
#include "ui_filterwidget.h"

FilterWidget::FilterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FilterWidget)
{
    ui->setupUi(this);

    connect(ui->lineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(filterChanged(QString)));
    connect(ui->closeButton, SIGNAL(clicked()), this, SIGNAL(closeFilter()));
    connect(ui->caseSensitive, SIGNAL(stateChanged(int)), this, SIGNAL(setCaseSensitive(int)));
}

FilterWidget::~FilterWidget()
{
    delete ui;
}

QLineEdit *FilterWidget::lineEdit()
{
    return ui->lineEdit;
}
