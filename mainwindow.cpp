#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_archive.h"
#include "ui_preferencesdialog.h"

#include "itemproxymodel.h"

#include <QtWidgets>
#include <QAbstractItemModel>
#include <QStringListModel>
#include <QtDebug>
#include <QXmlStreamWriter>
#include <QStack>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      archiveUi(new Ui::Archive),
      mDateFormat("MM/dd/yyyy")
{
    ui->setupUi(this);

    connect( &mUrgentImportant, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(itemChanged(QStandardItem*)) );
    connect( &mUrgentNotImportant, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(itemChanged(QStandardItem*)) );
    connect( &mNotUrgentImportant, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(itemChanged(QStandardItem*)) );
    connect( &mNotUrgentNotImportant, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(itemChanged(QStandardItem*)) );
    connect( &mArchive, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(itemChanged(QStandardItem*)) );

    ui->urgentImportant->setArchive(&mArchive);
    ui->urgentNotImportant->setArchive(&mArchive);
    ui->notUrgentImportant->setArchive(&mArchive);
    ui->notUrgentNotImportant->setArchive(&mArchive);

    mUrgentImportantProxy = new ItemProxyModel;
    mUrgentImportantProxy->setSourceModel( &mUrgentImportant );
    ui->urgentImportant->setModel( mUrgentImportantProxy );

    mUrgentNotImportantProxy = new ItemProxyModel;
    mUrgentNotImportantProxy->setSourceModel( &mUrgentNotImportant );
    ui->urgentNotImportant->setModel( mUrgentNotImportantProxy );

    mNotUrgentImportantProxy = new ItemProxyModel;
    mNotUrgentImportantProxy->setSourceModel( &mNotUrgentImportant );
    ui->notUrgentImportant->setModel( mNotUrgentImportantProxy );

    mNotUrgentNotImportantProxy = new ItemProxyModel;
    mNotUrgentNotImportantProxy->setSourceModel( &mNotUrgentNotImportant );
    ui->notUrgentNotImportant->setModel( mNotUrgentNotImportantProxy );

    QWidget *archiveWidget = new QWidget;
    archiveUi->setupUi(archiveWidget);

    mArchiveProxy = new ItemProxyModel;
    mArchiveProxy->setSourceModel( &mArchive );
    archiveUi->treeView->setModel(mArchiveProxy);

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

    connect(ui->urgentImportant,SIGNAL(save()),this,SLOT(writeXmlData()));
    connect(ui->urgentNotImportant,SIGNAL(save()),this,SLOT(writeXmlData()));
    connect(ui->notUrgentImportant,SIGNAL(save()),this,SLOT(writeXmlData()));
    connect(ui->notUrgentNotImportant,SIGNAL(save()),this,SLOT(writeXmlData()));
    connect(archiveUi->treeView,SIGNAL(save()),this,SLOT(writeXmlData()));

    mDataFolder = QDir::home();
    mDataFolder.mkdir("TaskManager");
    mDataFolder.cd("TaskManager");
    mDataFolder.setNameFilters( QStringList() << "TaskManager*.xml" );
    mDataFolder.setSorting(QDir::Name | QDir::Reversed);

    readXmlData();

    propagateDateTime();
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if( writeXmlData() ) {
        event->accept();
    } else {
        QMessageBox::critical(this, tr("Error saving tasks"), tr("There was an error saving the task list. Don't close the application right now."), QMessageBox::Ok);
        event->ignore();
    }
    cleanUpOldCopies();
}

void MainWindow::addItemsToModel(const QString &string, QStandardItemModel *model) const
{
    QStringList split = string.split("\n");
    for(int i=0; i<split.count(); i++)
    {
        QStringList tmp = split.at(i).split(QChar(0xfeff));
        if(tmp.count() < 2) { continue; }
        model->appendRow( newItem( static_cast<Qt::CheckState>(tmp.at(0).toInt()) == Qt::Checked , tmp.at(1) , mDateFormat ) );
    }
}

void MainWindow::serializeModel(QStandardItemModel *model, QXmlStreamWriter * stream, QTreeView * view) const
{
    for(int i=0; i<model->rowCount(); i++)
    {
        serializeItem( model, model->item(i), stream, view );
    }
}

void MainWindow::serializeItem(QStandardItemModel *model, QStandardItem *item, QXmlStreamWriter *stream, QTreeView *view) const
{
    stream->writeStartElement("task");

    stream->writeAttribute("expanded" , view->isExpanded( model->indexFromItem( item ) ) ? "yes" : "no" );

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

    QString url = item->data( MainWindow::Url ).toString();
    if( !url.isEmpty() )
    {
        stream->writeAttribute("href" , url );
    }

    for(int i=0; i<item->rowCount(); i++)
    {
        serializeItem( model, item->child(i), stream, view );
    }
    stream->writeEndElement(); // task
}

bool MainWindow::writeXmlData(QString path )
{
    if( path.isEmpty() )
    {
        path = dataFileWritePath();
    }
    QFile outFile( path );
    if( !outFile.open(QFile::WriteOnly | QFile::Text) )
        return false;

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
    serializeModel(&mUrgentImportant,&stream, ui->urgentImportant);
    stream.writeEndElement(); // urgent-important

    stream.writeStartElement("urgent-not-important");
    serializeModel(&mUrgentNotImportant,&stream, ui->urgentNotImportant);
    stream.writeEndElement(); // urgent-not-important

    stream.writeStartElement("not-urgent-important");
    serializeModel(&mNotUrgentImportant,&stream, ui->notUrgentImportant);
    stream.writeEndElement(); // not-urgent-important

    stream.writeStartElement("not-urgent-not-important");
    serializeModel(&mNotUrgentNotImportant,&stream, ui->notUrgentNotImportant);
    stream.writeEndElement(); // not-urgent-not-important

    stream.writeStartElement("archive");
    serializeModel( &mArchive,&stream, archiveUi->treeView );
    stream.writeEndElement(); // archive

    stream.writeEndElement(); // task-manager
    stream.writeEndDocument();

    return true;
}

void MainWindow::readXmlData(QString path )
{
    if( path.isEmpty() )
    {
        path = dataFileReadPath();
    }
    QFile file( path );
    if( ! file.open(QFile::ReadOnly) )
    {
        QMessageBox::critical(this, tr("Error reading file"), tr("There was an error saving the most recent task list: %1").arg(path), QMessageBox::Ok);
        return;
    }

    // clear the models
    mUrgentImportant.clear();
    mUrgentNotImportant.clear();
    mNotUrgentImportant.clear();
    mNotUrgentNotImportant.clear();
    mArchive.clear();

    QXmlStreamReader stream(&file);
    QStandardItemModel * currentModel = nullptr;
    QStack<QStandardItem*> currentItem;

    QList<QStandardItem*> * currentExpandedList = nullptr;
    QList<QStandardItem*> expandedUrgentImportant;
    QList<QStandardItem*> expandedUrgentNotImportant;
    QList<QStandardItem*> expandedNotUrgentImportant;
    QList<QStandardItem*> expandedNotUrgentNotImportant;

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
                currentExpandedList = &expandedUrgentImportant;
            }
            else if( name == "urgent-not-important" )
            {
                currentModel = &mUrgentNotImportant;
                currentExpandedList = &expandedUrgentNotImportant;
            }
            else if( name == "not-urgent-important" )
            {
                currentModel = &mNotUrgentImportant;
                currentExpandedList = &expandedNotUrgentImportant;
            }
            else if( name == "not-urgent-not-important" )
            {
                currentModel = &mNotUrgentNotImportant;
                currentExpandedList = &expandedNotUrgentNotImportant;
            }
            else if( name == "archive" )
            {
                currentModel = &mArchive;
            }
            else if( name == "task" )
            {
                QStandardItem * item = newItem( attributes.value("completed").toString() == "yes", attributes.value("label").toString() , mDateFormat, attributes.hasAttribute("date") ? QDateTime::fromString(attributes.value("date").toString(), Qt::ISODate ) : QDateTime() );

                if( attributes.hasAttribute("href") )
                {
                    item->setData( attributes.value("href").toString(), MainWindow::Url );
                }

                if( attributes.value("expanded").toString() == "yes" )
                {
                    currentExpandedList->append( item );
                }
                if( currentItem.isEmpty() )
                {
                    currentModel->appendRow( item );
                }
                else
                {
                    currentItem.top()->appendRow( item );
                }
                currentItem.push(item);
            }
        }
        else if( stream.tokenType() == QXmlStreamReader::EndElement )
        {
            if( name == "task" )
            {
                currentItem.pop();
            }
        }
    }

    foreach( QStandardItem * item, expandedUrgentImportant )
    {
        ui->urgentImportant->setExpanded( mUrgentImportant.indexFromItem( item ), true );
    }

    foreach( QStandardItem * item, expandedNotUrgentImportant )
    {
        ui->notUrgentImportant->setExpanded( mNotUrgentImportant.indexFromItem( item ), true );
    }

    foreach( QStandardItem * item, expandedUrgentNotImportant )
    {
        ui->urgentNotImportant->setExpanded( mUrgentNotImportant.indexFromItem( item ), true );
    }

    foreach( QStandardItem * item, expandedNotUrgentNotImportant )
    {
        ui->notUrgentNotImportant->setExpanded( mNotUrgentNotImportant.indexFromItem( item ), true );
    }
}

QString MainWindow::dataFileReadPath() const
{
    QStringList files = mDataFolder.entryList();
    if( files.size() > 0 )
    {
        return mDataFolder.absoluteFilePath( files.first() );
    }
    else
    {
        return "";
    }
}

QString MainWindow::dataFileWritePath() const
{
    return QDir::home().absoluteFilePath( QString("TaskManager/TaskManager-%1.xml").arg(QDateTime::currentDateTime().toString(Qt::ISODate).replace(":","-"))  );
}

void MainWindow::cleanUpOldCopies()
{
    QStringList files = mDataFolder.entryList();
    for(int i=20; i < files.size(); i++)
    {
        mDataFolder.remove( files.at(i) );
    }
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

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu(this);
    menu->addAction(tr("Open..."), this, SLOT(openFile()));
    menu->addAction(tr("Save as..."), this, SLOT(saveAs()));
    menu->addAction(tr("Open the data directory..."), this, SLOT(openDataDirectory()));

    menu->exec(event->globalPos());
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

void MainWindow::openFile()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Select file..."), mDataFolder.absolutePath(), tr("XML Files (*.xml)") );
    if( !path.isNull() )
    {
        readXmlData(path);
    }
}

void MainWindow::openDataDirectory()
{
    QDesktopServices::openUrl( QUrl::fromLocalFile( mDataFolder.absolutePath() ) );
}

void MainWindow::saveAs()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Select destination..."), mDataFolder.absolutePath(), tr("XML Files (*.xml)") );
    if( !path.isNull() )
    {
        writeXmlData(path);
    }
}

void MainWindow::propagateDateTime()
{
    ui->urgentImportant->setDateTimeFormat(mDateFormat);
    ui->urgentNotImportant->setDateTimeFormat(mDateFormat);
    ui->notUrgentImportant->setDateTimeFormat(mDateFormat);
    ui->notUrgentNotImportant->setDateTimeFormat(mDateFormat);
}
