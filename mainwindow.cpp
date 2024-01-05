#include "mainwindow.h"
#include <QStyle>
#include "./ui_mainwindow.h"
#include "videoplayer.h"
#include <format>
#include <iostream>
#include <span>

MainWindow::MainWindow(QApplication &qap, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , qap(qap)
    , live_points({})
{
    ui->setupUi(this);
    ui->filterDockWidget->layout()->setAlignment(Qt::AlignTop);
    ui->viewportWidget->set_supplier([this]() -> std::vector<std::vector<Point>> {
        return std::vector<std::vector<Point>>{
            std::vector<Point>(live_points.begin(), live_points.end())};
    });

    connect(this,
            &MainWindow::new_points,
            ui->viewportWidget,
            &PointCloudRenderer::point_supplier_update);

    bool srd_video = connect(this,
                             &MainWindow::new_rgb_data,
                             ui->videoPlayer,
                             &VideoPlayer::set_rgb_data);
    bool sdd_video = connect(this,
                             &MainWindow::new_depth_data,
                             ui->videoPlayer,
                             &VideoPlayer::set_depth_data);
    assert(srd_video);
    assert(sdd_video);

    bool tc_ok = connect(ui->captureList,
                         &CaptureList::take_capture,
                         this,
                         &MainWindow::take_capture);
    assert(tc_ok);

    bool newcapok = connect(this,
                            &MainWindow::new_capture,
                            ui->captureList,
                            &CaptureList::add_capture);
    bool showhideok = connect(this->ui->actionShow_Hide,
                              &QAction::triggered,
                              this->ui->captureList,
                              &CaptureList::toggle_show_hide);

    assert(newcapok);
    assert(showhideok);

    ui->captureRenderer->set_supplier(ui->captureList->points_supplier());

    bool psok = connect(ui->captureList,
                        &CaptureList::visible_captures_changed,
                        ui->captureRenderer,
                        &PointCloudRenderer::point_supplier_update);
    assert(psok);

    // Setup Statusbar (qt creator wont let you add widgets graphically)
    connection_status_label = new QLabel(this);
    connection_status_label->setTextFormat(Qt::MarkdownText);
    ui->statusbar->addPermanentWidget(connection_status_label);
    set_connection_status_ui(KinectConnectionStatus::Unknown);

    // Add dock control menu
    auto menu = createPopupMenu();
    if (menu == nullptr) {
        std::cout << "BAD" << std::endl;
    }
    menu->setTitle("Window");
    ui->menubar->addMenu(menu);

    try_connect_kinect();
}

void MainWindow::save_to(std::string path) {}
void MainWindow::save()
{
    if (project_path.has_value()) {
        save_to(project_path.value());
    }
}
void MainWindow::take_capture()
{
    if (freenect_device == nullptr) {
        return;
    }
    emit new_capture(freenect_device->take_capture());
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

// std::vector<Point> update_points_rgb(std::vector<Point> old_pts,
// std::span<uint8_t> new_data,
// VideoType video_mode)
// {
// if (old_pts.size() != 640 * 480) {
// old_pts.resize(640 * 480);
// }
// take our copy of data
// if (video_mode == VideoType::RGB) {
// for (size_t i = 0; i < old_pts.size(); i++) {
// old_pts[i].r = new_data[3 * i];
// old_pts[i].g = new_data[3 * i + 1];
// old_pts[i].b = new_data[3 * i + 2];
// }
// } else {
// for (size_t i = 0; i < old_pts.size(); i++) {
// old_pts[i].r = new_data[i];
// old_pts[i].g = new_data[i];
// old_pts[i].b = new_data[i];
// }
// }
// return old_pts;
// }
auto get_color(int i, std::span<uint8_t> rgb, VideoType vid_mode)
    -> std::tuple<uint8_t, uint8_t, uint8_t>
{
    if (vid_mode == VideoType::RGB) {
      auto r = rgb[3 * i];
      auto g = rgb[3 * i + 1];
      auto b = rgb[3 * i + 2];
      return {r, g, b};
    } else {
      auto g = rgb[i];
      return {g, g, g};
    }
}

std::vector<Point> update_points(std::vector<Point> old_pts,
                                 std::span<uint8_t> rgb_data,
                                 std::span<uint16_t> depth_data,
                                 VideoType video_mode,
                                 PointFilter::Filter filt)
{
    if (old_pts.size() != 640 * 480) {
      old_pts.resize(640 * 480);
    }

    // take our copy of data
    for (size_t i = 0; i < depth_data.size(); i++) {
      int x = i % 640;
      int y = i / 640;
      uint16_t depth = depth_data[i];

      auto [r, g, b] = get_color(i, rgb_data, video_mode);

      vec3 pos = MyFreenectDevice::pixel_to_point(x, y, depth);
      Point p = {.r = r, .g = g, .b = b, .pos = pos};
      if (filt(p)) {
          old_pts.push_back(p);
      }
    }
  return old_pts;
}

void MainWindow::try_connect_kinect(){
  if (freenect_device != nullptr) {
      // already connected
      return;
  }
  try {
    freenect_device = &freenect_ctx.createDevice<MyFreenectDevice>(0);

    freenect_device->install_color_callback([this](std::span<uint8_t> new_data) {
        auto filt = this->ui->ParentFilterSlot->get_filter();
        live_points = update_points(live_points,
                                    new_data,
                                    freenect_device->depth_data(),
                                    freenect_device->video_mode(),
                                    filt);

        // alert others
        emit new_rgb_data(new_data, freenect_device->video_mode());
        emit new_points();
    });

    freenect_device->install_depth_callback([this](std::span<uint16_t> new_data) {
        auto filt = this->ui->ParentFilterSlot->get_filter();

        live_points = update_points(live_points,
                                    freenect_device->color_data(),
                                    new_data,
                                    freenect_device->video_mode(),
                                    filt);

        // alert others
        emit new_depth_data(new_data);
        emit new_points();
    });

    freenect_device->startVideo();
    freenect_device->startDepth();

    freenect_device->updateState();
    double deg = freenect_device->getState().getTiltDegs();
    ui->angleSlider->setSliderPosition((int)deg);
    set_angle((int)deg);
    led_green();

    data_check_thread = std::thread{MainWindow::data_check_thread_runner, this};

    set_connection_status_ui(KinectConnectionStatus::Connected);
    emit kinect_connected();
  } catch (std::runtime_error er){
    freenect_device=nullptr;
    set_connection_status_ui(KinectConnectionStatus::Disconnected);
  }
}
void MainWindow::disconnect_kinect()
{
  if (freenect_device == nullptr) {
    // Wasnt connected anyways
    return;
  }
  thread_should_stop = true;
  data_check_thread.join();

  freenect_device->stopVideo();
  freenect_device->stopDepth();

  freenect_ctx.deleteDevice(0);
  freenect_device = nullptr;

  set_connection_status_ui(KinectConnectionStatus::Disconnected);

  // live_points = {};
  emit new_points();
  emit kinect_disconnected();
}

void MainWindow::quit()
{
  disconnect_kinect();
  std::cout << "quit\n"
            << std::endl;
  this->close();
}

std::string string_motion_code(freenect_tilt_status_code code){
  std::string s = "Moving";
  if (code == freenect_tilt_status_code::TILT_STATUS_LIMIT){
    s = "At Limit";
  } else if (code == freenect_tilt_status_code::TILT_STATUS_STOPPED) {
    s = "Stopped";
  }
  return s;
}

void MainWindow::data_check_thread_runner(MainWindow *win_ptr){
  MainWindow &win = *win_ptr;
  while (!win.thread_should_stop) {
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
void MainWindow::led_green()
{
  set_led(freenect_led_options::LED_GREEN);
}

void MainWindow::led_yellow(){
  set_led(freenect_led_options::LED_YELLOW);
}
void MainWindow::led_red()
{
  set_led(freenect_led_options::LED_RED);
}
void MainWindow::led_blink_green()
{
  set_led(freenect_led_options::LED_BLINK_GREEN);
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

void MainWindow::set_ir(int on)
{
  if (freenect_device->video_mode() == VideoType::RGB) {
    freenect_device->set_ir();
  } else {
    freenect_device->set_rgb();
  }
}

void MainWindow::set_angle(int angle)
{
  if (freenect_device == nullptr) {
    return;
  }

  freenect_device->setTiltDegrees((double) angle);
  ui->targetAngleLabel->setText(QString::fromStdString(std::format("Target: {} deg", angle)));
}

MainWindow::~MainWindow()
{
  delete ui;
}
