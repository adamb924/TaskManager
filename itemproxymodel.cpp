#include "itemproxymodel.h"
#include "mainwindow.h"

#include <QtDebug>
#include <QStandardItemModel>

ItemProxyModel::ItemProxyModel()
{

}

QVariant ItemProxyModel::data(const QModelIndex &index, int role) const
{
  if (role != Qt::ForegroundRole && role != Qt::FontRole && role != Qt::DisplayRole && role != Qt::EditRole)
    return QIdentityProxyModel::data(index, role);

  QStandardItemModel* model = qobject_cast<QStandardItemModel*>(sourceModel());
  QStandardItem * item = model->itemFromIndex( mapToSource( index ) );
  QString url = item->data(MainWindow::Url).toString();
  bool completed = item->data(MainWindow::Completed).toBool();

  if( role == Qt::ForegroundRole )
  {
      if( !url.isEmpty() )
      {
          return QBrush(QColor(Qt::blue));
      }
      else
      {
          return QIdentityProxyModel::data(index, role);
      }
  }
  else if( role == Qt::FontRole )
  {
      if( !url.isEmpty() )
      {
          QFont f;
          f.setUnderline(true);
          return f;
      }
      else
      {
          return QIdentityProxyModel::data(index, role);
      }
  }
  else if( role == Qt::DisplayRole )
  {
      if( completed )
      {
          return tr("%1 (%2)").arg( item->data(MainWindow::Label).toString() ).arg( item->data(MainWindow::Date).toDateTime().toString( mDateFormat ) );
      }
      else
      {
          return item->data(MainWindow::Label).toString();
      }
  }
  else if( role == Qt::EditRole )
  {
      return item->data(MainWindow::Label).toString();
  }

  return QVariant();
}

void ItemProxyModel::setDateTimeFormat(const QString &date)
{
    mDateFormat = date;
}
