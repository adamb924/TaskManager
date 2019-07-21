#ifndef LINKEDITDIALOG_H
#define LINKEDITDIALOG_H

#include <QDialog>

namespace Ui {
class LinkEditDialog;
}

class LinkEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LinkEditDialog(const QString & label = QString(), const QString & url = QString(), QWidget *parent = nullptr);
    ~LinkEditDialog();

    QString label() const;
    QString url() const;

private:
    Ui::LinkEditDialog *ui;
};

#endif // LINKEDITDIALOG_H
