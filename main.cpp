#include <QtWidgets>
#include <QtSql>

#include "ui_mainwindow.h"

class Settings : public QSettings
{
public:
    Settings()
        : QSettings(QSettings::UserScope,"SkypeVideoDownloader")
    {
    }
};

namespace {
}

struct ModelItem
{
  enum Status {Unknown = 0, Ready, InProgress, Finished, Error };

  ModelItem();

  QString id;
  QString vod_path;
  Status status;

};

ModelItem::ModelItem()
  : status(Unknown)
{
}

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

class MainWindow : public QWidget
{
  Q_OBJECT
public:
  MainWindow(QWidget *parent = 0);

private:
  void writeSettings();
  void readSettings();
  void closeEvent(QCloseEvent *event);

private slots:
  void onGetVideosClicked();
private:
  Ui::MainWindow ui;
  Model *model;
};

MainWindow::MainWindow(QWidget *parent)
  : QWidget(parent)
{
  ui.setupUi(this);

  connect(ui.pb_getVideos, SIGNAL(clicked(bool)), this, SLOT(onGetVideosClicked()));

  readSettings();

  model = new Model(this);
  ui.tv->setModel(model);
}

void MainWindow::writeSettings()
{
    Settings settings;

    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();

    settings.setValue("database_filename", ui.le_dbFilenname->text());
    settings.setValue("output_dir", ui.le_outputDir->text());
}

void MainWindow::readSettings()
{
    Settings settings;

    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(400, 400)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();

    ui.le_dbFilenname->setText(settings.value("database_filename"
          , "C:/Users/Admin/AppData/Roaming/Skype/alex-krutikov/main.db").toString());
    ui.le_outputDir->setText(settings.value("output_dir", "").toString());
}

void MainWindow::onGetVideosClicked()
{
    ui.label_statusline->clear();

    const QString filename = ui.le_dbFilenname->text();
    QFileInfo checkFile(filename);
    if (!checkFile.isFile())    {
        ui.label_statusline->setText("Cannot open database file.");
        return;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(filename);
    const bool ok = db.open();
    if (!ok) {
        ui.label_statusline->setText("Cannot read database file.");
        return;
    }

    QSqlQuery query;
    if (!query.exec("SELECT id, vod_path FROM VideoMessages")) {
        db.close();
        ui.label_statusline->setText("Cannot query video path from database.");
        return;
    }

    model->reset();

    while (query.next()) {
        const QString id = query.value(0).toString();
        const QString vodPath = query.value(1).toString();
        //qDebug() << id << vodPath;
        model->addItem(id, vodPath);
    }


    db.close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  MainWindow w;
  w.show();

  return app.exec();
}

#include "main.moc"
