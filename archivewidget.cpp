#include "archivewidget.h"

#include <QtWidgets>

ArchiveWidget::ArchiveWidget(QString init, QString date, QString time, QWidget *parent) :
    ListWidget(init,parent)
{
    dateFormat = date;
    timeFormat = time;
}

void ArchiveWidget::contextMenuEvent(QContextMenuEvent *e)
{
    QAction *remove = new QAction(tr("Delete this item"),this);
    QAction *preferences = new QAction(tr("Preferences"),this);
    QAction *save = new QAction(tr("Save"),this);

    connect(remove,SIGNAL(triggered()),this,SLOT(remove()));
    connect(preferences,SIGNAL(triggered()),this,SIGNAL(preferences()));
    connect(save,SIGNAL(triggered()),this,SIGNAL(save()));

    QMenu menu;
    menu.addAction(remove);
    menu.addAction(preferences);
    menu.addAction(save);
    menu.exec(e->globalPos());
}

void ArchiveWidget::addToArchive(QString text, Qt::CheckState state)
{
    QString tmp = text+" ("+QDate::currentDate().toString(dateFormat);
    if(timeFormat.length()>0)
    {
	tmp += " "+QTime::currentTime().toString(timeFormat);
    }
    tmp += ")";
    QListWidgetItem *tmpItem = new QListWidgetItem(tmp,0);
    this->insertItem(0,tmpItem);
    tmpItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt::ItemIsDragEnabled);
    tmpItem->setCheckState(state);
}

void ArchiveWidget::dropEvent(QDropEvent *event)
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

	addToArchive(text,(Qt::CheckState)state);
    }
}
