#ifndef DATAENTRYWIDGET_H
#define DATAENTRYWIDGET_H

#include <QDialog>
#include <QWidget>
#include <QList>
#include <QLineEdit>
#include <QList>
#include <QVariant>

class QStringList;

class DataEntryWidget : public QDialog
{
Q_OBJECT
public:
    explicit DataEntryWidget(QString ul, QString ur, QString ll, QString lr, QString dateFormat, QString timeFormat, QWidget *parent);

    QLineEdit *ul, *ur, *ll, *lr, *dateFormat, *timeFormat;

signals:

public slots:

};

#endif // DATAENTRYWIDGET_H
