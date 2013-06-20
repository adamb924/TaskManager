#include "listwidget.h"

#include <QListWidgetItem>
#include <QtWidgets>
#include <QStringList>

ListWidget::ListWidget(QString init, QWidget *parent) :
	QListWidget(parent)
{
    this->setDragDropMode(QAbstractItemView::DragDrop);
    this->setDragEnabled(true);
    this->setAcceptDrops(true);

    QStringList split = init.split("\n");
    for(int i=0; i<split.count(); i++)
    {
	QStringList tmp = split.at(i).split(QChar(0xfeff));
	if(tmp.count() < 2) { continue; }

	QListWidgetItem *tmpItem = new QListWidgetItem(tmp.at(1),this);
	tmpItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt::ItemIsDragEnabled);
	tmpItem->setCheckState((Qt::CheckState)tmp.at(0).toInt() );
    }
}

void ListWidget::contextMenuEvent(QContextMenuEvent *e)
{
    QAction *remove = new QAction(tr("Delete this item"),this);
    QAction *insert = new QAction(tr("Insert an item"),this);
    QAction *archive = new QAction(tr("Archive this item"),this);
    QAction *showArchive = new QAction(tr("Show the archive"),this);
    QAction *preferences = new QAction(tr("Preferences"),this);
    QAction *save = new QAction(tr("Save"),this);

    connect(insert,SIGNAL(triggered()),this,SLOT(insert()));
    connect(remove,SIGNAL(triggered()),this,SLOT(remove()));
    connect(archive,SIGNAL(triggered()),this,SLOT(archive()));
    connect(showArchive,SIGNAL(triggered()),this,SIGNAL(showArchive()));
    connect(preferences,SIGNAL(triggered()),this,SIGNAL(preferences()));
    connect(save,SIGNAL(triggered()),this,SIGNAL(save()));

    QMenu menu;
    if( this->itemAt(e->pos()) != 0 )
    {
	menu.addAction(remove);
	menu.addAction(archive);
    }
    menu.addAction(insert);
    menu.addSeparator();
    menu.addAction(showArchive);
    menu.addAction(preferences);
    menu.addAction(save);
    menu.exec(e->globalPos());
}

void ListWidget::archive()
{
    if(this->currentItem() != 0)
    {
	emit addToArchive(this->currentItem()->text(), this->currentItem()->checkState() );
	delete this->currentItem();
    }
}

void ListWidget::insert()
{
    QListWidgetItem *tmp = new QListWidgetItem("");
    this->insertItem(this->count(),tmp);
    tmp->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt::ItemIsDragEnabled);
    tmp->setCheckState(Qt::Unchecked);
    this->editItem(tmp);
}

void ListWidget::remove()
{
    if(this->currentItem() != 0)
    {
	delete this->currentItem();
    }
}

QString ListWidget::serialize()
{
    QString tmp = "";
    for(int i=0; i<this->count(); i++)
    {
	tmp += QString::number(this->item(i)->checkState()) + QChar(0xfeff) + this->item(i)->text() + "\n";
    }
    return tmp;
}

void ListWidget::dropEvent(QDropEvent *event)
{
    event->setDropAction(Qt::MoveAction);
    if(event->source() == this)
    {
	QListWidget::dropEvent(event);
    }
    else
    {
	QByteArray encodedData = event->mimeData()->data("text/plain");
	QDataStream stream(&encodedData, QIODevice::ReadOnly);

	int state;
	QString text;

	stream >> state;
	stream >> text;

	if(text.length() == 0 ) { return; }

	QListWidgetItem *tmp = new QListWidgetItem(text);
	this->insertItem(this->count(),tmp);
	tmp->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt::ItemIsDragEnabled);
	tmp->setCheckState((Qt::CheckState)state);
    }
}

void ListWidget::dragMoveEvent ( QDragMoveEvent *event )
{
    QListWidget::dragMoveEvent(event);
}

QStringList ListWidget::mimeTypes() const
{
    QStringList types;
    types << "text/plain";
    return types;
}

QMimeData* ListWidget::mimeData ( const QList<QListWidgetItem *> items ) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    for(int i=0; i<items.count(); i++)
    {
	stream << (int)items.at(i)->checkState();
	stream << items.at(i)->text();
    }

    mimeData->setData("text/plain", encodedData);
    return mimeData;
}
