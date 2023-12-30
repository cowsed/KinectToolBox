#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QApplication &qap, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), qap(qap)
{
  freenect_device = &freenect_ctx.createDevice<MyFreenectDevice>(0);
  freenect_device->startVideo();
  freenect_device->startDepth();


  ui->setupUi(this);
}


void MainWindow::quit(){
  freenect_device->stopVideo();
  freenect_device->stopDepth();

  std::cout << "rgb samples: "<<freenect_device->rgb_samples() <<'\n';
  std::cout << "depth samples: "<<freenect_device->depth_samples() <<'\n';
  std::cout << "quit\n"<<std::endl;
  freenect_ctx.deleteDevice(0);
  this->close();
}


MainWindow::~MainWindow()
{
  delete ui;
}
