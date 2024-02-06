#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include <QWidget>

class QLineEdit;

namespace Ui {
class FilterWidget;
}

class FilterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FilterWidget(QWidget *parent = nullptr);
    ~FilterWidget();

    QLineEdit * lineEdit();

signals:
    void filterChanged(const QString &);
    void closeFilter();
    void setCaseSensitive(int checkState);

private:
    Ui::FilterWidget *ui;
};

#endif // FILTERWIDGET_H
