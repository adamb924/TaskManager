#include "itemproxymodel.h"
#include "mainwindow.h"

#include <QtDebug>
#include <QStandardItemModel>
#include <QUrl>

ItemProxyModel::ItemProxyModel()
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setRecursiveFilteringEnabled(true);
}

QVariant ItemProxyModel::data(const QModelIndex &index, int role) const
{
  if (role != Qt::ForegroundRole && role != Qt::FontRole && role != Qt::DisplayRole && role != Qt::EditRole)
    return QSortFilterProxyModel::data(index, role);

  QStandardItemModel* model = qobject_cast<QStandardItemModel*>(sourceModel());
  QStandardItem * item = model->itemFromIndex( mapToSource( index ) );
  QUrl url = item->data(MainWindow::Url).toUrl();
  QDateTime completed = item->data(MainWindow::DateCompleted).toDateTime();
  bool putOnHold = item->data(MainWindow::PutOnHold).toBool();

  if( role == Qt::ForegroundRole )
  {
      return getItemColor( item );
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
          if( putOnHold )
          {
              QFont f;
              f.setItalic(true);
              return f;
          }
          else
          {
              return QSortFilterProxyModel::data(index, role);
          }
      }
  }
  else if( role == Qt::DisplayRole )
  {
      if( completed.isValid() )
      {
          return tr("%1 (%2)").arg( item->data(MainWindow::Label).toString() ).arg( item->data(MainWindow::DateCompleted).toDateTime().toString( mDateFormat ) );
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

QBrush ItemProxyModel::getItemColor(const QStandardItem * item) const
{
    QUrl url = item->data(MainWindow::Url).toUrl();
    bool putOnHold = item->data(MainWindow::PutOnHold).toBool();

    QColor color = putOnHold ? QColor(Qt::darkGray) : QColor(Qt::black);

    if( !url.isEmpty() ) /// this is a link
    {
        if( url.isLocalFile() )
        {
            color = putOnHold ? QColor(Qt::blue).lighter() : QColor(Qt::darkBlue);
        }
        else
        {
            color = putOnHold ? QColor(Qt::blue).lighter() : QColor(Qt::blue);
        }
    }

    return QBrush(color);
}
