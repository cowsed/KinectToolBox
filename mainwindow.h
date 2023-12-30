#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "myfreenectdevice.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QApplication &qap, QWidget *parent = nullptr);
  ~MainWindow();

public slots:
  void quit();

private:
  Ui::MainWindow *ui;
  QApplication &qap;
  Freenect::Freenect freenect_ctx;
  MyFreenectDevice *freenect_device;

};
#endif // MAINWINDOW_H
