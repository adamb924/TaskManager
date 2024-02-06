#include "macrotaskdialog.h"
#include "ui_macrotaskdialog.h"

#include "macrotask.h"

MacrotaskDialog::MacrotaskDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MacrotaskDialog)
{
    ui->setupUi(this);
}

MacrotaskDialog::MacrotaskDialog(const Macrotask &task, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MacrotaskDialog)
{
    ui->setupUi(this);
    ui->headerEdit->setText(task.header());
    ui->descriptionEdit->setText(task.description());
}

MacrotaskDialog::~MacrotaskDialog()
{
    delete ui;
}

Macrotask MacrotaskDialog::task() const
{
    return Macrotask( ui->headerEdit->text(), ui->descriptionEdit->toPlainText() );
}
