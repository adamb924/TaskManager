#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_archive.h"
#include "ui_preferencesdialog.h"

#include "itemproxymodel.h"
#include "list.h"

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

    QWidget *archiveWidget = new QWidget;
    archiveUi->setupUi(archiveWidget);

    mArchiveDock = new QDockWidget(tr("Archive"),this,Qt::Drawer);
    mArchiveDock->setWidget(archiveWidget);
    connect(archiveUi->close,SIGNAL(clicked()),mArchiveDock,SLOT(hide()));
    connect(archiveUi->removeAll,SIGNAL(clicked()),this,SLOT(removeAllFromArchive()));

    mLists[ MainWindow::UrgentImportant ] = new List( ui->urgentImportant,  "urgent-important" );
    mLists[ MainWindow::NotUrgentImportant ] = new List( ui->notUrgentImportant,  "not-urgent-important" );
    mLists[ MainWindow::UrgentNotImportant ] = new List( ui->urgentNotImportant,  "urgent-not-important" );
    mLists[ MainWindow::NotUrgentNotImportant ] = new List( ui->notUrgentNotImportant,  "not-urgent-not-important" );
    mLists[ MainWindow::Archive ] = new List( archiveUi->treeView,  "archive" );

    foreach( List * l, mLists )
    {
        l->proxy->setDateTimeFormat( mDateFormat );
        connect(l->model,SIGNAL( itemChanged(QStandardItem*) ),this,SLOT(itemChanged(QStandardItem*)));
        connect(l->view,SIGNAL(showArchive()),this,SLOT(showArchive()));
        connect(l->view,SIGNAL(preferences()),this,SLOT(preferences()));
        connect(l->view,SIGNAL(save()),this,SLOT(writeXmlData()));
        connect(l->view,SIGNAL(archiveItem(QStandardItem *,QString)),this,SLOT(archiveItem(QStandardItem *,QString)));

        connect(l->view,SIGNAL(openFile()),this,SLOT(openFile()));
        connect(l->view,SIGNAL(openDataDirectory()),this,SLOT(openDataDirectory()));
        connect(l->view,SIGNAL(saveAs()),this,SLOT(saveAs()));
    }

    mDataFolder = QDir::home();
    mDataFolder.mkdir("TaskManager");
    mDataFolder.cd("TaskManager");
    mDataFolder.setNameFilters( QStringList() << "TaskManager*.xml" );
    mDataFolder.setSorting(QDir::Name | QDir::Reversed);

    QSettings settings("AdamBaker", "TaskManager");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    readXmlData();

    propagateDateTime();
}

MainWindow::~MainWindow()
{
    qDeleteAll( mLists );
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

    QSettings settings("AdamBaker", "TaskManager");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
}

void MainWindow::serializeModel(List * list, QXmlStreamWriter *stream) const
{
    for(int i=0; i<list->model->rowCount(); i++)
    {
        serializeItem( list, list->model->item(i), stream );
    }
}

void MainWindow::serializeItem(List *list, QStandardItem *item, QXmlStreamWriter *stream) const
{
    stream->writeStartElement("task");

    stream->writeAttribute("expanded" , list->view->isExpanded( list->proxy->mapFromSource( list->model->indexFromItem( item ) ) ) ? "yes" : "no" );

    stream->writeAttribute("completed" , item->checkState() == Qt::Checked ? "yes" : "no" );

    stream->writeAttribute("label" , item->data( MainWindow::Label ).toString() );

    if( item->data( MainWindow::DateCompleted ).toDateTime().isValid() )
    {
        stream->writeAttribute("date-completed" , item->data( MainWindow::DateCompleted ).toDateTime().toString(Qt::ISODate) );
    }

    stream->writeAttribute("date-created" , item->data( MainWindow::DateCreated ).toDateTime().toString(Qt::ISODate) );

    stream->writeAttribute("origin-list" , item->data( MainWindow::OriginList ).toString() );

    QUrl url = item->data( MainWindow::Url ).toUrl();
    if( !url.isEmpty() )
    {
        stream->writeAttribute("href" , url.toString() );
    }

    for(int i=0; i<item->rowCount(); i++)
    {
        serializeItem( list, item->child(i), stream );
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

    foreach( List * l, mLists )
    {
        stream.writeStartElement( l->xmlString );
        serializeModel( l, &stream);
        stream.writeEndElement();
    }

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
    foreach( List * l, mLists )
    {
        l->model->clear();
    }

    QXmlStreamReader stream(&file);
    QStack<QStandardItem*> currentItem;

    MainWindow::ListType currentList;
    QHash<MainWindow::ListType, QList<QStandardItem*>> expandList;

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
                currentList = MainWindow::UrgentImportant;
            }
            else if( name == "urgent-not-important" )
            {
                currentList = MainWindow::UrgentNotImportant;
            }
            else if( name == "not-urgent-important" )
            {
                currentList = MainWindow::NotUrgentImportant;
            }
            else if( name == "not-urgent-not-important" )
            {
                currentList = MainWindow::NotUrgentNotImportant;
            }
            else if( name == "archive" )
            {
                currentList = MainWindow::Archive;
            }
            else if( name == "task" )
            {
                bool completed = attributes.value("completed").toString() == "yes";
                QString label = attributes.value("label").toString();
                QDateTime dateCompleted;
                if( attributes.hasAttribute("date") ) { // backwards compatibility
                    dateCompleted = QDateTime::fromString(attributes.value("date").toString(), Qt::ISODate );
                } else if( attributes.hasAttribute("date-completed") ) {
                    dateCompleted = QDateTime::fromString(attributes.value("date-completed").toString(), Qt::ISODate );
                }

                QDateTime dateCreated;
                if( attributes.hasAttribute("date-created") )
                {
                    dateCreated = QDateTime::fromString( attributes.value("date-created").toString(), Qt::ISODate);
                }

                QString href;
                if( attributes.hasAttribute("href") )
                {
                    href = attributes.value("href").toString();
                }

                QStandardItem * item = newItem( completed, label, dateCreated, dateCompleted, QUrl(href) );
                if( attributes.hasAttribute("origin-list") )
                {
                    item->setData( attributes.value("origin-list").toString(), MainWindow::OriginList );
                }

                if( attributes.value("expanded").toString() == "yes" )
                {
                    expandList[currentList].append( item );
                }
                if( currentItem.isEmpty() )
                {
                    mLists[currentList]->model->appendRow(item);
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

    QList<MainWindow::ListType> keys = expandList.keys();
    foreach( MainWindow::ListType key, keys )
    {
        foreach( QStandardItem * item, expandList.value(key) )
        {
            mLists[key]->view->setExpanded( mLists[key]->indexFromItem( item ), true );
        }
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

QStandardItem *MainWindow::newItem(bool completed, const QString &label, const QDateTime &dateCreated, const QDateTime &dateCompleted, const QUrl &url)
{
    QStandardItem * item = new QStandardItem();
    item->setData( false, MainWindow::JustChanged );
    item->setText( label );
    item->setData( label, MainWindow::Label );
    item->setData( url, MainWindow::Url );
    item->setData( dateCreated, MainWindow::DateCreated );
    if(dateCompleted.isValid())
    {
        item->setData( dateCompleted , MainWindow::DateCompleted );
    }
    else
    {
        item->setData( QDateTime() , MainWindow::DateCompleted );
    }
    item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled );
    if( completed )
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
        mLists[MainWindow::Archive]->model->clear();
    }
}

void MainWindow::openFile()
{
    if( QMessageBox::Yes == QMessageBox::question(this, tr("Task Manager"), tr("If you open another file, all of your current tasks will be lost. (You can always use ‘Save as...’ to save them someplace.) Is that what you want to do?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No ) )
    {
        QString path = QFileDialog::getOpenFileName(this, tr("Select file..."), mDataFolder.absolutePath(), tr("XML Files (*.xml)") );
        if( !path.isNull() )
        {
            readXmlData(path);
        }
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

void MainWindow::archiveItem(QStandardItem *item, const QString &origin)
{
    item->setData( origin, MainWindow::OriginList );
    mLists[MainWindow::Archive]->model->appendRow(item);
}

void MainWindow::propagateDateTime()
{
    foreach( List * l, mLists )
    {
        l->proxy->setDateTimeFormat( mDateFormat );
    }
}

void MainWindow::itemChanged(QStandardItem *item)
{
    bool justChanged = item->data(MainWindow::JustChanged).toBool();
    bool checked = item->checkState() == Qt::Checked;
    if( !justChanged && checked )
    {
        item->setData( true , MainWindow::JustChanged );
        item->setData( QDateTime::currentDateTime() , MainWindow::DateCompleted );
        item->setData( item->text() , MainWindow::Label );
    }
    else if ( justChanged && !checked )
    {
        item->setData( false , MainWindow::JustChanged );
        item->setData(  QDateTime() , MainWindow::DateCompleted );
        item->setData( item->text() , MainWindow::Label );
    }
    else
    {
        item->setData( item->text() , MainWindow::Label );
    }
}
