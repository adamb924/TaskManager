#include "mainwindow.h"

#include <QtWidgets>
#include <QAbstractItemModel>
#include <QStringListModel>
#include <QtDebug>

#include "listwidget.h"
#include "archivewidget.h"
#include "dataentrywidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    settings = new QSettings("TaskManager", "Adam Baker");

    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;
    urgentImportant = new ListWidget(settings->value("urgent_important").toString());
    urgentNotImportant = new ListWidget(settings->value("urgent_notimportant").toString());
    notUrgentImportant = new ListWidget(settings->value("noturgent_important").toString());
    notUrgentNotImportant = new ListWidget(settings->value("noturgent_notimportant").toString());
    archive = new ArchiveWidget(settings->value("archive").toString(),dateFormat,timeFormat);

    archiveDock = new QDockWidget(tr("Archive"),this,Qt::Drawer);
    QVBoxLayout *archiveLayout = new QVBoxLayout;
    QPushButton *close = new QPushButton(tr("Close"));
    QPushButton *deleteAll = new QPushButton(tr("Remove all"));
    archiveLayout->addWidget(archive);
    archiveLayout->addWidget(close);
    archiveLayout->addWidget(deleteAll);
    QWidget *archiveWidget = new QWidget;
    archiveWidget->setLayout(archiveLayout);
    archiveDock->setWidget(archiveWidget);
    connect(close,SIGNAL(clicked()),archiveDock,SLOT(hide()));
    connect(deleteAll,SIGNAL(clicked()),this,SLOT(removeAllFromArchive()));

    urgentImportant->setStyleSheet("QListWidget{ border: 3px solid red; }");
    urgentNotImportant->setStyleSheet("QListWidget{ border: 3px solid brown; }");
    notUrgentImportant->setStyleSheet("QListWidget{ border: 3px solid blue; }");
    notUrgentNotImportant->setStyleSheet("QListWidget{ border: 3px solid green; }");

    connect(urgentImportant,SIGNAL(addToArchive(QString,Qt::CheckState)),archive,SLOT(addToArchive(QString,Qt::CheckState)));
    connect(urgentNotImportant,SIGNAL(addToArchive(QString,Qt::CheckState)),archive,SLOT(addToArchive(QString,Qt::CheckState)));
    connect(notUrgentImportant,SIGNAL(addToArchive(QString,Qt::CheckState)),archive,SLOT(addToArchive(QString,Qt::CheckState)));
    connect(notUrgentNotImportant,SIGNAL(addToArchive(QString,Qt::CheckState)),archive,SLOT(addToArchive(QString,Qt::CheckState)));

    connect(urgentImportant,SIGNAL(showArchive()),this,SLOT(showArchive()));
    connect(urgentNotImportant,SIGNAL(showArchive()),this,SLOT(showArchive()));
    connect(notUrgentImportant,SIGNAL(showArchive()),this,SLOT(showArchive()));
    connect(notUrgentNotImportant,SIGNAL(showArchive()),this,SLOT(showArchive()));

    connect(urgentImportant,SIGNAL(preferences()),this,SLOT(preferences()));
    connect(urgentNotImportant,SIGNAL(preferences()),this,SLOT(preferences()));
    connect(notUrgentImportant,SIGNAL(preferences()),this,SLOT(preferences()));
    connect(notUrgentNotImportant,SIGNAL(preferences()),this,SLOT(preferences()));
    connect(archive,SIGNAL(preferences()),this,SLOT(preferences()));

    connect(urgentImportant,SIGNAL(save()),this,SLOT(saveData()));
    connect(urgentNotImportant,SIGNAL(save()),this,SLOT(saveData()));
    connect(notUrgentImportant,SIGNAL(save()),this,SLOT(saveData()));
    connect(notUrgentNotImportant,SIGNAL(save()),this,SLOT(saveData()));
    connect(archive,SIGNAL(save()),this,SLOT(saveData()));

    dateFormat = settings->value("date_format","MM/dd/yyyy").toString();
    timeFormat = settings->value("time_format","").toString();

    archive->setDateFormat(dateFormat);
    archive->setTimeFormat(timeFormat);

    ul = new QLabel( settings->value("ul_label",tr("Urgent & Important")).toString() );
    layout->addWidget(ul,0,0);
    layout->addWidget(urgentImportant,1,0);

    ur = new QLabel( settings->value("ur_label",tr("Urgent, Not Important")).toString() );
    layout->addWidget(ur,0,1);
    layout->addWidget(urgentNotImportant,1,1);

    ll = new QLabel( settings->value("ll_label",tr("Important, Not Urgent")).toString() );
    layout->addWidget(ll,2,0);
    layout->addWidget(notUrgentImportant,3,0);

    lr = new QLabel( settings->value("lr_label",tr("Follow up at some point")).toString() );
    layout->addWidget(lr,2,1);
    layout->addWidget(notUrgentNotImportant,3,1);

    widget->setLayout(layout);

    this->setCentralWidget(widget);

    this->setWindowTitle(tr("Task Manager"));
}

MainWindow::~MainWindow()
{
    delete settings;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveData();
    event->accept();
}

void MainWindow::saveData()
{
    settings->setValue("urgent_important", urgentImportant->serialize() );
    settings->setValue("urgent_notimportant", urgentNotImportant->serialize() );
    settings->setValue("noturgent_important", notUrgentImportant->serialize() );
    settings->setValue("noturgent_notimportant", notUrgentNotImportant->serialize() );
    settings->setValue("archive", archive->serialize() );

    settings->setValue("ul_label", ul->text() );
    settings->setValue("ur_label", ur->text() );
    settings->setValue("ll_label", ll->text() );
    settings->setValue("lr_label", lr->text() );

    settings->setValue("date_format", dateFormat );
    settings->setValue("time_format", timeFormat );
}

void MainWindow::showArchive()
{
    archiveDock->show();
}

void MainWindow::preferences()
{
    DataEntryWidget dew(ul->text(),ur->text(),ll->text(),lr->text(), dateFormat, timeFormat, this);
    if(dew.exec()==QDialog::Accepted)
    {
	ul->setText( dew.ul->text() );
	ur->setText( dew.ur->text() );
	ll->setText( dew.ll->text() );
	lr->setText( dew.lr->text() );

	dateFormat = dew.dateFormat->text();
	timeFormat = dew.timeFormat->text();

	archive->setDateFormat(dateFormat);
	archive->setTimeFormat(timeFormat);
    }
}

void MainWindow::removeAllFromArchive()
{
    if( QMessageBox::Yes == QMessageBox::question(this, tr("Task Manager"), tr("Are you sure you want to clear the archive?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No ) )
	archive->clear();
}
