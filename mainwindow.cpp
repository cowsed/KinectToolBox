#include "mainwindow.h"
#include <QStyle>
#include "./ui_mainwindow.h"
#include "videoplayer.h"
#include <format>
#include <iostream>
#include <span>

PointCloud::Ptr update_points(std::span<rgb> rgb_data,
                              std::span<uint16_t> depth_data,
                              const PointFilter::Filter &filt)
{
    PointCloud::Ptr newcloud = std::make_shared<PointCloud>();
    newcloud->reserve(num_pixels);

    // take our copy of data
    for (size_t i = 0; i < depth_data.size(); i++) {
        size_t image_x = i % kinect_video_width;
        size_t image_y = i / kinect_video_width;

        uint16_t depth = depth_data[i];

        auto [r, g, b] = rgb_data[i];
        auto [x, y, z] = MyFreenectDevice::pixel_to_point(image_x, image_y, depth);

        Point point(x, y, z, r, g, b);

        if (depth != 0 && filt(point)) {
            newcloud->push_back(point);
        }
    }
    return newcloud;
}

MainWindow::MainWindow(QApplication &qap, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , qap(qap)
    , live_points(update_points, std::make_shared<PointCloud>(), [this]() { emit new_points(); })
{
    ui->setupUi(this);
    ui->filterDockWidget->layout()->setAlignment(Qt::AlignTop);
    ui->viewportWidget->set_supplier([this]() -> std::vector<PointCloud::Ptr> {
        return std::vector<PointCloud::Ptr>{live_points.get_result()};
    });

    connect(this,
            &MainWindow::new_points,
            ui->viewportWidget,
            &PointCloudRenderer::point_supplier_update);

    connect(this, &MainWindow::new_rgb_data, ui->videoPlayer, &VideoPlayer::set_rgb_data);
    connect(this, &MainWindow::new_depth_data, ui->videoPlayer, &VideoPlayer::set_depth_data);
    connect(ui->captureList, &CaptureList::take_capture, this, &MainWindow::take_capture);

    connect(this, &MainWindow::new_capture, ui->captureList, &CaptureList::add_capture);

    connect(this->ui->actionShow_Hide,
            &QAction::triggered,
            this->ui->captureList,
            &CaptureList::toggle_show_hide);

    ui->captureRenderer->set_supplier(ui->captureList->points_supplier());

    connect(ui->captureList,
            &CaptureList::visible_captures_changed,
            ui->captureRenderer,
            &PointCloudRenderer::point_supplier_update);

    // Setup Statusbar (qt creator wont let you add widgets graphically)
    connection_status_label = new QLabel(this);
    connection_status_label->setTextFormat(Qt::MarkdownText);
    ui->statusbar->addPermanentWidget(connection_status_label);
    set_connection_status_ui(KinectConnectionStatus::Unknown);

    // Add dock control menu
    auto *menu_of_docks = createPopupMenu();
    if (menu_of_docks == nullptr) {
        std::cout << "BAD" << std::endl;
    } else {
        menu_of_docks->setTitle("Window");
        ui->menubar->addMenu(menu_of_docks);
    }
    try_connect_kinect();
}

void MainWindow::save_to(const std::string &path) {}
void MainWindow::save()
{
    if (project_path.has_value()) {
        save_to(project_path.value());
    }
}
void MainWindow::export_captures()
{
    fd->setFileMode(QFileDialog::FileMode::Directory);
    fd->open();
    connect(fd, &QFileDialog::fileSelected, this, &MainWindow::export_captures_to_directory);
}
void MainWindow::export_captures_to_directory(const QString &dir)
{
    disconnect(fd, &QFileDialog::fileSelected, this, &MainWindow::export_captures_to_directory);
    ui->captureList->save_all(dir);
}

void MainWindow::take_capture()
{
    if (freenect_device == nullptr) {
        return;
    }

    auto video_capture = freenect_device->take_capture();
    auto filt = this->ui->ParentFilterSlot->get_filter();

    emit new_capture(video_capture, filt);
}

void MainWindow::set_connection_status_ui(KinectConnectionStatus stat){
  switch(stat){
    case KinectConnectionStatus::Connected:
      connection_status_label->setText("Connected :)");
      connection_status_label->setStyleSheet("color: green");
      enable_connect();
      break;
    case KinectConnectionStatus::Disconnected:
      connection_status_label->setText("Disconnected :(");
      connection_status_label->setStyleSheet("color: red");
      disable_disconnect();
      break;
    case KinectConnectionStatus::Unknown:
      connection_status_label->setText("Connection Unknown");
      connection_status_label->setStyleSheet("color: yellow");
      disable_disconnect();
      break;
    }
}

void MainWindow::disable_disconnect()
{
    ui->captureDock->setDisabled(true);
    ui->controlDockWidget->setDisabled(true);
    ui->filterDockWidget->setDisabled(true);
    ui->videoDockWidget->setDisabled(true);
    ui->actionDisconnect->setDisabled(true);
    ui->actionReconnect->setDisabled(false);
}
void MainWindow::enable_connect()
{
    ui->captureDock->setDisabled(false);
    ui->controlDockWidget->setDisabled(false);
    ui->filterDockWidget->setDisabled(false);
    ui->videoDockWidget->setDisabled(false);

    ui->actionDisconnect->setDisabled(false);
    ui->actionReconnect->setDisabled(true);
}

static auto get_color(size_t pixel_index, std::span<uint8_t> rgb)
    -> std::tuple<uint8_t, uint8_t, uint8_t>
{
    return {
        rgb[3 * pixel_index],
        rgb[3 * pixel_index + 1],
        rgb[3 * pixel_index + 2],
    };
}
static auto get_ir(size_t pixel_index, std::span<uint8_t> rgb)
    -> std::tuple<uint8_t, uint8_t, uint8_t>
{
    return {
        rgb[pixel_index],
        rgb[pixel_index],
        rgb[pixel_index],
    };
}


void MainWindow::try_connect_kinect(){
  if (freenect_device != nullptr) {
      // already connected
      return;
  }
  try {
    freenect_device = &freenect_ctx.createDevice<MyFreenectDevice>(0);

    freenect_device->install_color_callback([this](std::span<rgb> new_data) {
        live_points.add_work(new_data,
                             freenect_device->depth_data(),
                             ui->ParentFilterSlot->get_filter());
        // alert others
        emit new_rgb_data(new_data);
    });

    freenect_device->install_depth_callback([this](std::span<uint16_t> new_data) {
        live_points.add_work(freenect_device->color_data(),
                             new_data,
                             ui->ParentFilterSlot->get_filter());
        emit new_depth_data(new_data);
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
  constexpr size_t thread_sleep_time_ms = 100;
  MainWindow &win = *win_ptr;
  while (!win.thread_should_stop) {
    win.freenect_device->updateState();
    auto state = win.freenect_device->getState();

    {
        std::scoped_lock lock{win.data_mtx};
        win.tilt_state = state;
    }

    double deg = state.getTiltDegs();
    auto degree_text = std::format("Current: {}", deg);
    constexpr double bad_deg_reading = -64;

    if (deg == bad_deg_reading) {
        degree_text = "Current:" + string_motion_code(state.m_code);
    }
    double imux;
    double imuy;
    double imuz;
    state.getAccelerometers(&imux, &imuy, &imuz);
    auto imu_txt = std::format("Imu: {:.1f} {:.1f} {:.1f}", imux, imuy, imuz);

    win.ui->curAngleLabel->setText(QString::fromStdString(degree_text));
    win.ui->accLabel->setText(QString::fromStdString(imu_txt));

    std::this_thread::sleep_for(std::chrono::milliseconds(thread_sleep_time_ms));
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
  if (freenect_device == nullptr) {
    return;
  }
  if (freenect_device->video_mode() == MyFreenectDevice::VideoType::RGB) {
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
  disconnect_kinect();
  delete ui;
}
