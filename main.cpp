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
};

MainWindow::MainWindow(QWidget *parent)
  : QWidget(parent)
{
  ui.setupUi(this);

  connect(ui.pb_getVideos, SIGNAL(clicked(bool)), this, SLOT(onGetVideosClicked()));

  readSettings();
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
    while (query.next()) {
        const QString id = query.value(0).toString();
        const QString vodPath = query.value(1).toString();
        qDebug() << id << vodPath;
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
