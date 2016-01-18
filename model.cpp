#include "model.h"

#include <QtWidgets>
#include <QtSql>
#include <QtNetwork>

ModelItem::ModelItem()
  : status(Unknown)
{
}

Model::Model(QObject *parent)
  : QAbstractTableModel(parent)
  , reply(NULL)
  , itemsIndex(0)
{
  networkManager = new QNetworkAccessManager(this);
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
      switch (index.column())  {
      case 0:
          return items.at(index.row()).id;
      case 1:
          return items.at(index.row()).progress;
      case 2:
          return items.at(index.row()).vod_path;

      }
    case Qt::BackgroundRole:
      switch (items.at(index.row()).status)  {
      case ModelItem::Unknown:
        return QVariant();
      case ModelItem::Ready:
        return QColor("gray");
      case ModelItem::AuthFailed:
        return QColor("pink");
      case ModelItem::InProgress:
        return QColor("yellow");
      case ModelItem::Finished:
        return QColor("green");
      case ModelItem::Error:
        return QColor("red");
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
      return "progress";
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
  initItemStatus(&item);
  items << item;
}

void Model::initItemStatus(ModelItem *item)
{
  const QFileInfo fi(outputDir + "/" + QString(item->id) + ".mp4");
  if (fi.exists()) {
    item->status = ModelItem::Finished;
    return;
 }

 if (item->vod_path.isEmpty()) {
    item->status = ModelItem::Error;
    return;
 }
 item->status = ModelItem::Ready;
}

void Model::startItemDownloading()
{
  do {
    if (itemsIndex >= items.size())
      return;

    const ModelItem &item = items.at(itemsIndex);
    if ((item.status != ModelItem::Ready)
      ||(item.status != ModelItem::AuthFailed))
      break;

  itemsIndex++;
  } while(true);


  if (reply)
    reply->deleteLater();

  reply = networkManager->get(QNetworkRequest(QUrl(items.at(itemsIndex).vod_path)));
  connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(onDownloadProgress(qint64,qint64)));
  connect(reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
  connect(reply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
  connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
  connect(reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(slotSslErrors(QList<QSslError>)));
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

void Model::downloadVideos()
{
  itemsIndex = 0;
  startItemDownloading();
}

void Model::setOutputDir(const QString &outputDir)
{
  this->outputDir = outputDir;
}

void Model::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
  items[itemsIndex].progress = QString("%1K of %2K").arg(bytesReceived/1024).arg(bytesTotal/1024);
  emit dataChanged(index(itemsIndex,0), index(itemsIndex, columnCount() - 1));
}

void Model::onReadyRead()
{
  items[itemsIndex].status = ModelItem::InProgress;
  emit dataChanged(index(itemsIndex,0), index(itemsIndex, columnCount() - 1));
}

void Model::onReplyFinished()
{
  reply->disconnect();
  reply->deleteLater();
  reply = NULL;

  items[itemsIndex].status = ModelItem::Finished;
  emit dataChanged(index(itemsIndex,0), index(itemsIndex, columnCount() - 1));

  itemsIndex++;
  startItemDownloading();
}

void Model::slotError(QNetworkReply::NetworkError error)
{
  switch (error) {
  case QNetworkReply::AuthenticationRequiredError:
    items[itemsIndex].status = ModelItem::AuthFailed;
    break;
  default:
    items[itemsIndex].status = ModelItem::Error;
  }

  emit dataChanged(index(itemsIndex,0), index(itemsIndex, columnCount() - 1));

  reply->disconnect();
  reply->deleteLater();
  reply = NULL;

  itemsIndex++;
  startItemDownloading();
}

void Model::slotSslErrors(QList<QSslError> errors)
{
  reply->disconnect();
  reply->deleteLater();
  reply = NULL;

  items[itemsIndex].status = ModelItem::Error;
  emit dataChanged(index(itemsIndex,0), index(itemsIndex, columnCount() - 1));

  itemsIndex++;
  startItemDownloading();
}
