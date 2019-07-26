#include "linkeditdialog.h"
#include "ui_linkeditdialog.h"

#include <QUrl>

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

QUrl LinkEditDialog::url() const
{
    return QUrl(ui->urlEdit->text());
}
