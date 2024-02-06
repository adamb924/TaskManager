#ifndef MACROTASKDIALOG_H
#define MACROTASKDIALOG_H

#include <QDialog>

namespace Ui {
class MacrotaskDialog;
}

class Macrotask;

class MacrotaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MacrotaskDialog(QWidget *parent = nullptr);
    MacrotaskDialog(const Macrotask & task, QWidget *parent = nullptr);
    ~MacrotaskDialog() override;

    Macrotask task() const;

private:
    Ui::MacrotaskDialog *ui;
};

#endif // MACROTASKDIALOG_H
