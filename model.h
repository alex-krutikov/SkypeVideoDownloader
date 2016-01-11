#ifndef __MODEL_H__
#define __MODEL_H__

#include <QAbstractTableModel>

struct ModelItem
{
  enum Status {Unknown = 0, Ready, InProgress, Finished, Error };

  ModelItem();

  QString id;
  QString vod_path;
  Status status;

};


class Model : public QAbstractTableModel
{
public:
  Model(QObject *parent = 0);
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

  void reset();
  void addItem(const QString &id, const QString &vod_path);

private:
  QVector<ModelItem> items;
};

#endif // __MODEL_H__
