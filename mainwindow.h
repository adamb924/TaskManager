#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>

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

    enum DataType { Label = Qt::UserRole+1, Date };

public slots:
    void showArchive();
    void preferences();
    void saveData();
    void removeAllFromArchive();

private:
    Ui::MainWindow *ui;
    QSettings *mSettings;
    QString mDateFormat;

    QStandardItemModel mUrgentImportant, mUrgentNotImportant, mNotUrgentImportant, mNotUrgentNotImportant, mArchive;

    QDockWidget *mArchiveDock;


    void closeEvent(QCloseEvent *event);
    void addItemsToModel(const QString & string, QStandardItemModel * model) const;
    void serializeModel(QStandardItemModel * model, QXmlStreamWriter *stream) const;
    void serializeItem(QStandardItem * item, QXmlStreamWriter *stream) const;
    void propagateDateTime();
    void readData();
    QString dataFilePath() const;
};

#endif // MAINWINDOW_H
