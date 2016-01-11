#include "model.h"

#include <QtWidgets>
#include <QtSql>

ModelItem::ModelItem()
  : status(Unknown)
{
}

Model::Model(QObject *parent)
  : QAbstractTableModel(parent)
{
}

int Model::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return items.count();
}

int Model::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 3;
}

QVariant Model::data(const QModelIndex &index, int role) const
{
  switch (role)
  {
    case Qt::DisplayRole:
      switch (index.column())
      {
      case 0:
          return items.at(index.row()).id;
      case 1:
          return items.at(index.row()).vod_path;
      case 2:
          return items.at(index.row()).vod_path;

      }

  }
  return QVariant();
}

QVariant Model::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Vertical) {
    if (role == Qt::DisplayRole )
      return section + 1;
    return QVariant();
  }

  switch (role)
  {
    case Qt::DisplayRole:
    switch (section) {
    case 0:
      return "id";
    case 1:
      return "status";
    case 2:
      return "vod_path";
    }
  }
  return QVariant();
}

void Model::reset()
{
  beginResetModel();
  items.clear();
  endResetModel();
}

void Model::addItem(const QString &id, const QString &vod_path)
{
  ModelItem item;
  item.id = id;
  item.vod_path = vod_path;
  items << item;
}

void Model::readDatabase(const QString &databaseFile)
{
    QFileInfo checkFile(databaseFile);
    if (!checkFile.isFile())    {
        emit errorMessage("Cannot open database file.");
        return;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(databaseFile);
    const bool ok = db.open();
    if (!ok) {
        emit errorMessage("Cannot read database file.");
        return;
    }

    QSqlQuery query;
    if (!query.exec("SELECT id, vod_path FROM VideoMessages")) {
        db.close();
        emit errorMessage("Cannot query video path from database.");
        return;
    }

    reset();

    while (query.next()) {
        const QString id = query.value(0).toString();
        const QString vodPath = query.value(1).toString();
        //qDebug() << id << vodPath;
        addItem(id, vodPath);
    }


    db.close();

}
