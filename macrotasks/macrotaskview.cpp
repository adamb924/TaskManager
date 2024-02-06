#include "macrotaskview.h"
#include "macrotask.h"
#include "qevent.h"

#include <QMenu>
#include <QContextMenuEvent>
#include <QMessageBox>
#include <QtDebug>

#include "macrotaskdialog.h"

MacrotaskView::MacrotaskView(QWidget *parent) : QListView(parent)
{

}

void MacrotaskView::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu menu;

    QModelIndex index = indexAt(e->pos());
    if( index.isValid() )
    {
        menu.addAction(tr("Edit task..."), this, [&, this](){ editMacrotask( index.row() ); });
    }

    menu.addAction(tr("Add task here..."), this, [&, this](){ addMacrotask( index.isValid() ? index.row() : model()->rowCount() ); });

    if( index.isValid() )
    {
        menu.addAction(tr("Remove task"), this, [&, this](){ removeMacrotask( index.row() ); });
    }

    menu.exec(e->globalPos());
}

void MacrotaskView::addMacrotask(int where)
{
    MacrotaskDialog dlg(this);
    if( dlg.exec() == QDialog::Accepted )
    {
        model()->insertRow(where);
        QModelIndex index = model()->index(where,0);
        model()->setData(index, QVariant::fromValue( dlg.task() ) );
    }
}

void MacrotaskView::editMacrotask(int which)
{
    QModelIndex index = model()->index(which,0); /// there ought only ever to be one anyway
    MacrotaskDialog dlg( qvariant_cast<Macrotask>(index.data()),  this);
    if( dlg.exec() == QDialog::Accepted )
    {
        model()->setData(index, QVariant::fromValue( dlg.task() ) );
    }
}

void MacrotaskView::removeMacrotask(int which)
{
    QModelIndex index = model()->index(which,0); /// there ought only ever to be one anyway
    Macrotask task = qvariant_cast<Macrotask>(index.data());

    if( QMessageBox::question(this, tr("Confirm deletion"), tr("Are you sure you want to delete the task '%1'?").arg(task.header()))
        == QMessageBox::Yes )
    {
        model()->removeRow( index.row() );
    }
}
