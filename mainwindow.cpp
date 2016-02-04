#include "mainwindow.h"
#include "model.h"

#include "ui_mainwindow.h"

#include <QtWidgets>
#include <QtSql>

namespace {

class Settings : public QSettings
{
public:
    Settings()
        : QSettings(QSettings::UserScope,"SkypeVideoDownloader")
    {
    }
};

} // namespace

MainWindow::MainWindow(QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  readSettings();

  model = new Model(this);
  ui->tv->setModel(model);

  connect(ui->pb_getVideos, &QPushButton::clicked, this, &MainWindow::onGetVideosClicked);
  connect(ui->pb_downloadVideos, &QPushButton::clicked, this, &MainWindow::onDownloadClicked);
  connect(ui->le_outputDir, &QLineEdit::textChanged, this, &MainWindow::onOutputDirChanged);
  connect(model, &Model::errorMessage, this, &MainWindow::onErrorMessage);

  model->setOutputDir(ui->le_outputDir->text());
}

void MainWindow::writeSettings()
{
    Settings settings;

    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();

    settings.setValue("database_filename", ui->le_dbFilenname->text());
    settings.setValue("output_dir", ui->le_outputDir->text());
}

void MainWindow::readSettings()
{
    Settings settings;

    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(400, 400)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();

    ui->le_dbFilenname->setText(settings.value("database_filename"
          , "C:/Users/Admin/AppData/Roaming/Skype/alex-krutikov/main.db").toString());
    ui->le_outputDir->setText(settings.value("output_dir", "").toString());
}

void MainWindow::onGetVideosClicked()
{
    ui->label_statusline->clear();
    model->readDatabase(ui->le_dbFilenname->text());
}

void MainWindow::onDownloadClicked()
{
    ui->label_statusline->clear();
    model->downloadVideos();
}

void MainWindow::onOutputDirChanged(const QString &outputDir)
{
  model->setOutputDir(outputDir);
}

void MainWindow::onErrorMessage(const QString &message)
{
    ui->label_statusline->setText(message);
    ui->label_statusline->setStyleSheet("color: red");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}
