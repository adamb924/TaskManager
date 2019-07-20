#include "itemproxymodel.h"
#include "mainwindow.h"

#include <QtDebug>

ItemProxyModel::ItemProxyModel()
{

}

QVariant ItemProxyModel::data(const QModelIndex &index, int role) const
{
  if (role != Qt::ForegroundRole && role != Qt::FontRole)
    return QIdentityProxyModel::data(index, role);


  QString url = sourceModel()->itemData( mapToSource( index ) ).value(MainWindow::Url).toString();
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

  return QVariant();
}
