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
    explicit LinkEditDialog(const QString & label = QString(), const QString & url = QString(), bool focusUrl = false, QWidget *parent = nullptr);
    ~LinkEditDialog() override;

    QString label() const;
    QUrl url() const;

private:
    Ui::LinkEditDialog *ui;
};

#endif // LINKEDITDIALOG_H
