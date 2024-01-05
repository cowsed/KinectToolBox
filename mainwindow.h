#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "myfreenectdevice.h"
#include "qlabel.h"
#include <future>
#include <thread>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


enum class KinectConnectionStatus{
  Connected,
  Disconnected,
  Unknown,
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
  MainWindow(QApplication &qap, QWidget *parent = nullptr);
  ~MainWindow();

  void save_to(std::string path);

  public slots:
      void quit();

      void take_capture();

      void try_connect_kinect();
      void disconnect_kinect();
      /**
     * @brief set_connection_status_ui
     * @param stat status of the connection, changes the representation in the status bar
     */
      void set_connection_status_ui(KinectConnectionStatus stat);

      void set_angle(int angle);
      void save();

      /*
   * Set LED States  
   */
      void led_off();
      void led_green();
      void led_yellow();
      void led_red();
      void led_blink_green();
      void led_blink_red_yellow();
      void set_led(freenect_led_options opt);

      void set_ir(int on);

  signals:
  void new_rgb_data(std::span<uint8_t> data, VideoType typ);
  void new_depth_data(std::span<uint16_t> data);
  void new_points();
  void kinect_connected();
  void kinect_disconnected();
  void new_capture(VideoCapture, PointCloud::Ptr);

  private:
      std::optional<std::string> project_path;

      static void data_check_thread_runner(MainWindow* win);
      Ui::MainWindow* ui;
      QApplication& qap;
      Freenect::Freenect freenect_ctx;
      MyFreenectDevice* freenect_device = nullptr;
      PointCloud::Ptr live_points;

      std::thread data_check_thread;
      std::mutex data_mtx;
      std::optional<Freenect::FreenectTiltState> tilt_state;
      bool thread_should_stop = false;

      // needed here cuz status bar widgets are weird
      QLabel* connection_status_label;
};
#endif // MAINWINDOW_H
