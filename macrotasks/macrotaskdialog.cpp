#include "macrotaskdialog.h"
#include "ui_macrotaskdialog.h"

#include "macrotask.h"

#include <QColorDialog>
#include <QDebug>

MacrotaskDialog::MacrotaskDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MacrotaskDialog),
    mColor(QColor(255,255,255))
{
    ui->setupUi(this);
}

MacrotaskDialog::MacrotaskDialog(const Macrotask &task, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MacrotaskDialog),
    mColor(task.color())
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
    return Macrotask( ui->headerEdit->text(), ui->descriptionEdit->toPlainText(), mColor );
}

void MacrotaskDialog::chooseBackgroundColor()
{
    QColor c = QColorDialog::getColor(mColor, nullptr, tr("Choose background color") );
    if( c.isValid() )
    {
        mColor = c;
    }
}
