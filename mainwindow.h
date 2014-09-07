#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QDateTime>

class QSettings;
class QDialog;
class QDockWidget;
class QLabel;
class QXmlStreamWriter;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    enum DataType { Label = Qt::UserRole+1, Date, JustChanged };

public slots:
    void showArchive();
    void preferences();
    void saveXmlData();
    void removeAllFromArchive();

    void itemChanged(QStandardItem * item);

private:
    Ui::MainWindow *ui;
    QString mDateFormat;

    QStandardItemModel mUrgentImportant, mUrgentNotImportant, mNotUrgentImportant, mNotUrgentNotImportant, mArchive;

    QDockWidget *mArchiveDock;


    void closeEvent(QCloseEvent *event);
    void addItemsToModel(const QString & string, QStandardItemModel *model) const;
    void serializeModel(QStandardItemModel * model, QXmlStreamWriter *stream) const;
    void serializeItem(QStandardItem * item, QXmlStreamWriter *stream) const;
    void propagateDateTime();
    void readXmlData();
    void readSettingsData();
    QString dataFilePath() const;
    QStandardItem* newItem(bool checked, const QString & label, const QDateTime & date = QDateTime() ) const;
};

#endif // MAINWINDOW_H
