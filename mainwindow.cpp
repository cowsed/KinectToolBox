#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>
#include <format>
#include <QStyle>

MainWindow::MainWindow(QApplication &qap, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), qap(qap)
{
  ui->setupUi(this);
  connection_status_label = new QLabel(this);
  connection_status_label->setTextFormat(Qt::MarkdownText);
  ui->statusbar->addPermanentWidget(connection_status_label);
  set_connection_status_ui(KinectConnectionStatus::Unknown);

  try_connect_kinect();

}

void MainWindow::set_connection_status_ui(KinectConnectionStatus stat){
  switch(stat){
    case KinectConnectionStatus::Connected:
      connection_status_label->setText("Connected :)");
      connection_status_label->setStyleSheet("color: green");
      break;
    case KinectConnectionStatus::Disconnected:
      connection_status_label->setText("Disconnected :(");
      connection_status_label->setStyleSheet("color: red");
      break;
    case KinectConnectionStatus::Unknown:
      connection_status_label->setText("Connection Unknown");
      connection_status_label->setStyleSheet("color: yellow");
      break;
  }
}

void MainWindow::try_connect_kinect(){
  try {
    freenect_device = &freenect_ctx.createDevice<MyFreenectDevice>(0);
    freenect_device->startVideo();
    freenect_device->startDepth();

    freenect_device->updateState();
    double deg = freenect_device->getState().getTiltDegs();
    ui->angleSlider->setSliderPosition((int)deg);
    set_angle((int)deg);
    led_green();

    data_check_thread = std::thread{MainWindow::data_check_thread_runner, this};
    ui->viewportWidget->set_device(freenect_device);

    set_connection_status_ui(KinectConnectionStatus::Connected);
  } catch (std::runtime_error er){
    freenect_device=nullptr;
    set_connection_status_ui(KinectConnectionStatus::Disconnected);
  }
}

void MainWindow::set_angle(int angle){
  if (freenect_device == nullptr){
    return;
  }

  freenect_device->setTiltDegrees((double)angle);
  ui->targetAngleLabel->setText(QString::fromStdString(std::format("Target: {} deg", angle)));
}

void MainWindow::quit(){
  thread_should_stop = true;
  if (freenect_device!=nullptr){
    freenect_device->stopVideo();
    freenect_device->stopDepth();
  }

  std::cout << "rgb samples: "<<freenect_device->rgb_samples() <<'\n';
  std::cout << "depth samples: "<<freenect_device->depth_samples() <<'\n';
  std::cout << "quit\n"<<std::endl;
  data_check_thread.join();
  this->close();
}

std::string string_motion_code(freenect_tilt_status_code code){
  std::string s = "Moving";
  if (code == freenect_tilt_status_code::TILT_STATUS_LIMIT){
    s = "At Limit";
  } else if (code == freenect_tilt_status_code::TILT_STATUS_STOPPED){
    s = "Stopped";
  }
  return s;
}

void MainWindow::data_check_thread_runner(MainWindow *win_ptr){
  MainWindow &win = *win_ptr;
  while(!win.thread_should_stop){
    win.freenect_device->updateState();
    auto state = win.freenect_device->getState();
    {
      std::scoped_lock l{win.data_mtx};
      win.tilt_state = state;
    }

    double deg = state.getTiltDegs();
    auto txt = std::format("Current: {}", deg);
    if (deg == -64){
      txt = "Current:" + string_motion_code(state.m_code);
    }
    win.ui->curAngleLabel->setText(QString::fromStdString(txt));
    double x, y, z;
    state.getAccelerometers(&x, &y, &z);
    auto txt2 = std::format("Imu: {:.1f} {:.1f} {:.1f}", x,y,z);
    win.ui->accLabel->setText(QString::fromStdString(txt2));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void MainWindow::led_off(){
  set_led(freenect_led_options::LED_OFF);
}
void MainWindow::led_green(){
  set_led(freenect_led_options::LED_GREEN);

}
void MainWindow::led_yellow(){
  set_led(freenect_led_options::LED_YELLOW);

}
void MainWindow::led_red(){
  set_led(freenect_led_options::LED_RED);

}
void MainWindow::led_blink_green(){  set_led(freenect_led_options::LED_BLINK_GREEN);
}
void MainWindow::led_blink_red_yellow(){
  set_led(freenect_led_options::LED_BLINK_RED_YELLOW);
}
void MainWindow::set_led(freenect_led_options opt){
  if (freenect_device == nullptr){
    return;
  }
  freenect_device->setLed(opt);
}

MainWindow::~MainWindow()
{
  delete ui;
}
