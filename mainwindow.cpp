#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_archive.h"
#include "ui_preferencesdialog.h"

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

    connect( &mUrgentImportant, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(itemChanged(QStandardItem*)) );
    connect( &mUrgentNotImportant, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(itemChanged(QStandardItem*)) );
    connect( &mNotUrgentImportant, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(itemChanged(QStandardItem*)) );
    connect( &mNotUrgentNotImportant, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(itemChanged(QStandardItem*)) );

    ui->urgentImportant->setArchive(&mArchive);
    ui->urgentNotImportant->setArchive(&mArchive);
    ui->notUrgentImportant->setArchive(&mArchive);
    ui->notUrgentNotImportant->setArchive(&mArchive);

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

    connect(ui->urgentImportant,SIGNAL(save()),this,SLOT(saveXmlData()));
    connect(ui->urgentNotImportant,SIGNAL(save()),this,SLOT(saveXmlData()));
    connect(ui->notUrgentImportant,SIGNAL(save()),this,SLOT(saveXmlData()));
    connect(ui->notUrgentNotImportant,SIGNAL(save()),this,SLOT(saveXmlData()));
    connect(archiveUi->treeView,SIGNAL(save()),this,SLOT(saveXmlData()));

    if( QFileInfo::exists( dataFilePath() ) )
    {
        readXmlData();
    }
    else
    {
        readSettingsData();
    }

    propagateDateTime();
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveXmlData();
    event->accept();
}

void MainWindow::addItemsToModel(const QString &string, QStandardItemModel *model) const
{
    QStringList split = string.split("\n");
    for(int i=0; i<split.count(); i++)
    {
        QStringList tmp = split.at(i).split(QChar(0xfeff));
        if(tmp.count() < 2) { continue; }
        model->appendRow( newItem( (Qt::CheckState)tmp.at(0).toInt() == Qt::Checked , tmp.at(1) , mDateFormat ) );
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
    if( item->data( MainWindow::Date ).toDateTime().isValid() )
    {
        stream->writeAttribute("label" , item->data( MainWindow::Label ).toString() );
        stream->writeAttribute("date" , item->data( MainWindow::Date ).toDateTime().toString(Qt::ISODate) );
    }
    else
    {
        stream->writeAttribute("label", item->text() );
    }
    for(int i=0; i<item->rowCount(); i++)
    {
        serializeItem( item->child(i), stream );
    }
    stream->writeEndElement(); // task
}

void MainWindow::saveXmlData()
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

void MainWindow::readXmlData()
{
    QFile file(dataFilePath());
    file.open(QFile::ReadOnly);
    QXmlStreamReader stream(&file);
    QStandardItemModel * currentModel = 0;
    QStandardItem * currentItem = 0;

    while (!stream.atEnd())
    {
        stream.readNext();
        QString name = stream.name().toString();
        QXmlStreamAttributes attributes = stream.attributes();
        if( stream.tokenType() == QXmlStreamReader::StartElement )
        {
            if( name == "ul-label" )
            {
                ui->ul->setText( stream.readElementText() );
            }
            else if( name == "ur-label" )
            {
                ui->ur->setText( stream.readElementText() );
            }
            else if( name == "ll-label" )
            {
                ui->ll->setText( stream.readElementText() );
            }
            else if( name == "lr-label" )
            {
                ui->lr->setText( stream.readElementText() );
            }
            else if( name == "date-format" )
            {
                mDateFormat = stream.readElementText();
                propagateDateTime();
            }
            else if( name == "urgent-important" )
            {
                currentModel = &mUrgentImportant;
            }
            else if( name == "urgent-not-important" )
            {
                currentModel = &mUrgentNotImportant;
            }
            else if( name == "not-urgent-important" )
            {
                currentModel = &mNotUrgentImportant;
            }
            else if( name == "not-urgent-not-important" )
            {
                currentModel = &mNotUrgentNotImportant;
            }
            else if( name == "task" )
            {
                QStandardItem * item = newItem( attributes.value("completed").toString() == "yes", attributes.value("label").toString() , mDateFormat, attributes.hasAttribute("date") ? QDateTime::fromString(attributes.value("date").toString(), Qt::ISODate ) : QDateTime() );
                if( currentItem == 0 )
                {
                    currentModel->appendRow( item );
                }
                else
                {
                    currentItem->appendRow( item );
                }
                currentItem = item;
            }
        }
        else if( stream.tokenType() == QXmlStreamReader::EndElement )
        {
            if( name == "task" )
            {
                currentItem = 0;
            }
        }
    }
}

void MainWindow::readSettingsData()
{
    QSettings settings("TaskManager", "Adam Baker");
    mDateFormat = settings.value("date_format","MM/dd/yyyy").toString();
    ui->ul->setText( settings.value("ul_label",tr("Urgent & Important")).toString() );
    ui->ur->setText( settings.value("ur_label",tr("Urgent, Not Important")).toString() );
    ui->ll->setText( settings.value("ll_label",tr("Important, Not Urgent")).toString() );
    ui->lr->setText( settings.value("lr_label",tr("Follow up at some point")).toString() );
    addItemsToModel( settings.value("urgent_important").toString() , &mUrgentImportant);
    addItemsToModel( settings.value("urgent_notimportant").toString() , &mUrgentNotImportant);
    addItemsToModel( settings.value("noturgent_important").toString() , &mNotUrgentImportant );
    addItemsToModel( settings.value("noturgent_notimportant").toString() , &mNotUrgentNotImportant );
}

QString MainWindow::dataFilePath() const
{
    return QDir::home().absoluteFilePath("TaskManager.xml");
}

QStandardItem *MainWindow::newItem(bool checked, const QString &label, const QString & dateFormat, const QDateTime &date)
{
    QStandardItem * item = new QStandardItem();
    item->setData( false, MainWindow::JustChanged );
    item->setData( label, MainWindow::Label );
    if(date.isValid())
    {
        item->setData( date , MainWindow::Date );
        item->setText( tr("%1 (%2)").arg( label ).arg( date.toString( dateFormat ) ) );
    }
    else
    {
        item->setData( 0 , MainWindow::Date );
        item->setText( label );
    }
    item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled );
    if( checked )
    {
        item->setData( true, MainWindow::JustChanged );
        item->setCheckState( Qt::Checked );
    }
    else
    {
        item->setData( false, MainWindow::JustChanged );
        item->setCheckState( Qt::Unchecked );
    }
    return item;
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

void MainWindow::itemChanged(QStandardItem *item)
{
    if( item->checkState() == Qt::Checked && item->data(MainWindow::JustChanged).toBool() == false )
    {
        item->setData( true , MainWindow::JustChanged );
        item->setData( item->text() , MainWindow::Label );
        item->setData( QDateTime::currentDateTime() , MainWindow::Date );
        item->setText( tr("%1 (%2)").arg( item->data(MainWindow::Label).toString() ).arg( item->data(MainWindow::Date).toDateTime().toString( mDateFormat ) ) );
    }
    else if( item->checkState() == Qt::Unchecked && item->data(MainWindow::JustChanged).toBool() == true )
    {
        item->setData( false , MainWindow::JustChanged );
        item->setData( 0 , MainWindow::Date );
        item->setText( item->data(MainWindow::Label).toString() );
    }
}

void MainWindow::propagateDateTime()
{
    ui->urgentImportant->setDateTimeFormat(mDateFormat);
    ui->urgentNotImportant->setDateTimeFormat(mDateFormat);
    ui->notUrgentImportant->setDateTimeFormat(mDateFormat);
    ui->notUrgentNotImportant->setDateTimeFormat(mDateFormat);
}
