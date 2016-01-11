#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QWidget>

#include <memory>

namespace Ui {
  class MainWindow;
}

class Model;

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
  void onErrorMessage(const QString &message);
private:
  std::auto_ptr<Ui::MainWindow> ui;
  Model *model;
};

#endif // __MAINWINDOW_H__
