#include <QtWidgets>
#include <QtSql>

#include "ui_mainwindow.h"

class MainWindow : public QWidget
{
  Q_OBJECT
public:
  MainWindow(QWidget *parent = 0)
    : QWidget(parent)
  {
    ui.setupUi(this);

    ui.databaseFile->setText("C:/Users/Admin/AppData/Roaming/Skype/alex-krutikov/main.db");

    connect(ui.databaseGetVideos, SIGNAL(clicked(bool)), this, SLOT(onGetVideosClicked()));

    resize(600, 800);
  }
private slots:
  void onGetVideosClicked();
private:
  Ui::MainWindow ui;
};

void MainWindow::onGetVideosClicked()
{
    const QString filename = ui.databaseFile->text();
    QFileInfo checkFile(filename);
    if (!checkFile.isFile())
      return;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(filename);
    const bool ok = db.open();
    if (!ok)
      return;

    QSqlQuery query;
    query.exec("SELECT id, vod_path FROM VideoMessages");
    while (query.next()) {
        const QString id = query.value(0).toString();
        const QString vodPath = query.value(1).toString();
        qDebug() << id << vodPath;
    }


    db.close();
}

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  MainWindow w;
  w.show();

  return app.exec();
}

#include "main.moc"
