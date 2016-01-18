#ifndef __MODEL_H__
#define __MODEL_H__

#include <QAbstractTableModel>
#include <QNetworkReply>

class QNetworkAccessManager;
class QNetworkReply;

struct ModelItem
{
  enum Status {Unknown = 0, Ready, AuthFailed,InProgress, Finished, Error };

  ModelItem();

  QString id;
  QString progress;
  QString vod_path;
  Status status;

};


class Model : public QAbstractTableModel
{
  Q_OBJECT
public:
  Model(QObject *parent = 0);
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

  void reset();
  void readDatabase(const QString &databaseFile);
  void downloadVideos();
  void setOutputDir(const QString &outputDir);

private slots:
  void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
  void onReadyRead();
  void onReplyFinished();
  void slotError(QNetworkReply::NetworkError);
  void slotSslErrors(QList<QSslError>);

private:
  void addItem(const QString &id, const QString &vod_path);
  void initItemStatus(ModelItem *item);
  void startItemDownloading();

signals:
  void errorMessage(const QString &message);

private:
  QVector<ModelItem> items;
  QString outputDir;
  QNetworkAccessManager *networkManager;
  QNetworkReply *reply;
  int itemsIndex;
};

#endif // __MODEL_H__
