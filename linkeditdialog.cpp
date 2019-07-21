#include "linkeditdialog.h"
#include "ui_linkeditdialog.h"

LinkEditDialog::LinkEditDialog(const QString &label, const QString &url, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LinkEditDialog)
{
    ui->setupUi(this);
    ui->labelEdit->setText(label);
    ui->urlEdit->setText(url);
}

LinkEditDialog::~LinkEditDialog()
{
    delete ui;
}

QString LinkEditDialog::label() const
{
    return ui->labelEdit->text();
}

QString LinkEditDialog::url() const
{
    return ui->urlEdit->text();
}
