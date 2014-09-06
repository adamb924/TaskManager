#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_archive.h"
#include "ui_preferencesdialog.h"

#include "taskitem.h"

#include <QtWidgets>
#include <QAbstractItemModel>
#include <QStringListModel>
#include <QtDebug>
#include <QXmlStreamWriter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      mDateFormat("MM/dd/yyyy")
{
    ui->setupUi(this);

    mSettings = new QSettings("TaskManager", "Adam Baker");

    mUrgentImportant.setItemPrototype( new TaskItem("Prototype") );
    mUrgentNotImportant.setItemPrototype( new TaskItem("Prototype") );
    mNotUrgentImportant.setItemPrototype( new TaskItem("Prototype") );
    mNotUrgentNotImportant.setItemPrototype( new TaskItem("Prototype") );

    ui->urgentImportant->setArchive(&mArchive);
    ui->urgentNotImportant->setArchive(&mArchive);
    ui->notUrgentImportant->setArchive(&mArchive);
    ui->notUrgentNotImportant->setArchive(&mArchive);

    addItemsToModel( mSettings->value("urgent_important").toString() , &mUrgentImportant);
    addItemsToModel( mSettings->value("urgent_notimportant").toString() , &mUrgentNotImportant);
    addItemsToModel( mSettings->value("noturgent_important").toString() , &mNotUrgentImportant );
    addItemsToModel( mSettings->value("noturgent_notimportant").toString() , &mNotUrgentNotImportant );

    ui->urgentImportant->setModel( &mUrgentImportant );
    ui->urgentNotImportant->setModel( &mUrgentNotImportant );
    ui->notUrgentImportant->setModel( &mNotUrgentImportant );
    ui->notUrgentNotImportant->setModel( &mNotUrgentNotImportant );

    QWidget *archiveWidget = new QWidget;
    Ui::Archive *archiveUi = new Ui::Archive;
    archiveUi->setupUi(archiveWidget);
    archiveUi->treeView->setModel(&mArchive);
    mArchiveDock = new QDockWidget(tr("Archive"),this,Qt::Drawer);
    mArchiveDock->setWidget(archiveWidget);
    connect(archiveUi->close,SIGNAL(clicked()),mArchiveDock,SLOT(hide()));
    connect(archiveUi->removeAll,SIGNAL(clicked()),this,SLOT(removeAllFromArchive()));

    connect(ui->urgentImportant,SIGNAL(showArchive()),this,SLOT(showArchive()));
    connect(ui->urgentNotImportant,SIGNAL(showArchive()),this,SLOT(showArchive()));
    connect(ui->notUrgentImportant,SIGNAL(showArchive()),this,SLOT(showArchive()));
    connect(ui->notUrgentNotImportant,SIGNAL(showArchive()),this,SLOT(showArchive()));

    connect(ui->urgentImportant,SIGNAL(preferences()),this,SLOT(preferences()));
    connect(ui->urgentNotImportant,SIGNAL(preferences()),this,SLOT(preferences()));
    connect(ui->notUrgentImportant,SIGNAL(preferences()),this,SLOT(preferences()));
    connect(ui->notUrgentNotImportant,SIGNAL(preferences()),this,SLOT(preferences()));
    connect(archiveUi->treeView,SIGNAL(preferences()),this,SLOT(preferences()));

    connect(ui->urgentImportant,SIGNAL(save()),this,SLOT(saveData()));
    connect(ui->urgentNotImportant,SIGNAL(save()),this,SLOT(saveData()));
    connect(ui->notUrgentImportant,SIGNAL(save()),this,SLOT(saveData()));
    connect(ui->notUrgentNotImportant,SIGNAL(save()),this,SLOT(saveData()));
    connect(archiveUi->treeView,SIGNAL(save()),this,SLOT(saveData()));

    propagateDateTime();
}

MainWindow::~MainWindow()
{
    delete mSettings;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveData();
    event->accept();
}

void MainWindow::addItemsToModel(const QString &string, QStandardItemModel *model) const
{
    QStringList split = string.split("\n");
    for(int i=0; i<split.count(); i++)
    {
        QStringList tmp = split.at(i).split(QChar(0xfeff));
        if(tmp.count() < 2) { continue; }

        TaskItem * item = new TaskItem( tmp.at(1) );
//        item->setCheckState((Qt::CheckState)tmp.at(0).toInt() );
        model->appendRow(item);

//        QStandardItem * item = new TaskItem( tmp.at(1) );
//        item->setCheckState((Qt::CheckState)tmp.at(0).toInt() );
//        model->appendRow(item);

    }
}

void MainWindow::serializeModel(QStandardItemModel *model, QXmlStreamWriter * stream) const
{
    for(int i=0; i<model->rowCount(); i++)
    {
        serializeItem( model->item(i) , stream );
    }
}

void MainWindow::serializeItem(QStandardItem *item, QXmlStreamWriter *stream) const
{
    stream->writeStartElement("task");
    stream->writeAttribute("completed" , item->checkState() == Qt::Checked ? "yes" : "no" );
    stream->writeAttribute("label" , item->data(MainWindow::Label).toString() );
    if( item->data().toDateTime().isValid() )
    {
        stream->writeAttribute("date", item->data(MainWindow::Date).toDateTime().toString(Qt::ISODate) );
    }
    for(int i=0; i<item->rowCount(); i++)
    {
        serializeItem( item->child(i), stream );
    }
    stream->writeEndElement(); // task
}

void MainWindow::saveData()
{
    QFile outFile( dataFilePath() );
    if( !outFile.open(QFile::WriteOnly | QFile::Text) )
        return;

    QXmlStreamWriter stream(&outFile);
    stream.setCodec("UTF-8");
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("task-manager");

    stream.writeTextElement("ul-label", ui->ul->text() );
    stream.writeTextElement("ur-label", ui->ur->text() );
    stream.writeTextElement("ll-label", ui->ll->text() );
    stream.writeTextElement("lr-label", ui->lr->text() );
    stream.writeTextElement("date-format", mDateFormat );

    stream.writeStartElement("urgent-important");
    serializeModel(&mUrgentImportant,&stream);
    stream.writeEndElement(); // urgent-important

    stream.writeStartElement("urgent-not-important");
    serializeModel(&mUrgentNotImportant,&stream);
    stream.writeEndElement(); // urgent-not-important

    stream.writeStartElement("not-urgent-important");
    serializeModel(&mNotUrgentImportant,&stream);
    stream.writeEndElement(); // not-urgent-important

    stream.writeStartElement("not-urgent-not-important");
    serializeModel(&mNotUrgentNotImportant,&stream);
    stream.writeEndElement(); // not-urgent-not-important

    stream.writeStartElement("archive");
    serializeModel(&mArchive,&stream);
    stream.writeEndElement(); // archive

    stream.writeEndElement(); // task-manager
    stream.writeEndDocument();
}

void MainWindow::readData()
{
    QFile file(dataFilePath());
    file.open(QFile::ReadOnly);
    QXmlStreamReader stream(&file);

    while (!stream.atEnd())
    {
        stream.readNext();
        QString name = stream.name().toString();
        if( stream.tokenType() == QXmlStreamReader::StartElement )
        {
            if( name == "ul-label" )
            {
            }
        }
    }
}

QString MainWindow::dataFilePath() const
{
    return QDir::home().absoluteFilePath("TaskManager.xml");
}

void MainWindow::showArchive()
{
    mArchiveDock->show();
}

void MainWindow::preferences()
{
    QDialog *dlg = new QDialog;
    Ui::PreferencesDialog *preferencesUi = new Ui::PreferencesDialog;
    preferencesUi->setupUi(dlg);
    preferencesUi->ul->setText( ui->ul->text() );
    preferencesUi->ur->setText( ui->ur->text() );
    preferencesUi->ll->setText( ui->ll->text() );
    preferencesUi->lr->setText( ui->lr->text() );
    preferencesUi->date->setText( mDateFormat );
    if( dlg->exec() )
    {
        ui->ul->setText( preferencesUi->ul->text() );
        ui->ur->setText( preferencesUi->ur->text() );
        ui->ll->setText( preferencesUi->ll->text() );
        ui->lr->setText( preferencesUi->lr->text() );

        mDateFormat = preferencesUi->date->text();
        propagateDateTime();
    }
}

void MainWindow::removeAllFromArchive()
{
    if( QMessageBox::Yes == QMessageBox::question(this, tr("Task Manager"), tr("Are you sure you want to clear the archive?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No ) )
    {
        mArchive.clear();
    }
}

void MainWindow::propagateDateTime()
{
    ui->urgentImportant->setDateTimeFormat(mDateFormat);
    ui->urgentNotImportant->setDateTimeFormat(mDateFormat);
    ui->notUrgentImportant->setDateTimeFormat(mDateFormat);
    ui->notUrgentNotImportant->setDateTimeFormat(mDateFormat);
}
