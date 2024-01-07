#include "videoplayer.h"
#include "ui_videoplayer.h"
#include <iostream>

constexpr QSize frame_size(kinect_video_width, kinect_video_height);

std::array<rgb, rgb_buffer_size> default_rgb_image_data;
std::array<uint16_t, depth_buffer_size> default_depth_image_data;

VideoPlayer::VideoPlayer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VideoPlayer)
    , depth_img((uchar *) default_depth_image_data.data(),
                frame_size.width(),
                frame_size.height(),
                QImage::Format_Grayscale16)
    , rgb_img((uchar *) default_rgb_image_data.data(),
              frame_size.width(),
              frame_size.height(),
              QImage::Format_RGB888)
    , do_updates(true)
{
    ui->setupUi(this);

    ui->depthLabel->setPixmap(QPixmap::fromImage(depth_img));
    ui->rgbLabel->setPixmap(QPixmap::fromImage(rgb_img));
}
void VideoPlayer::start()
{
    do_updates = true;
}
void VideoPlayer::reset()
{

    do_updates = false;

    rgb_img = QImage((uchar *) default_rgb_image_data.data(),
                     frame_size.width(),
                     frame_size.height(),
                     QImage::Format_RGB888);
    depth_img = QImage((uchar *) default_depth_image_data.data(),
                       frame_size.width(),
                       frame_size.height(),
                       QImage::Format_Grayscale16);

    ui->rgbLabel->setPixmap(QPixmap::fromImage(rgb_img));
    ui->depthLabel->setPixmap(QPixmap::fromImage(depth_img));
}

void VideoPlayer::set_rgb_data(std::span<rgb> data)
{
    if (!do_updates) {
        return;
    }
    QImage::Format fmt = QImage::Format_RGB888;

    rgb_img = QImage((uchar *) data.data(), frame_size.width(), frame_size.height(), fmt);
    ui->rgbLabel->setPixmap(QPixmap::fromImage(rgb_img));
}
void VideoPlayer::set_depth_data(std::span<uint16_t> data)
{
    if (!do_updates) {
        return;
    }

    const auto min_millis = 300;
    const auto max_millis = 3500;
    const auto divider = (max_millis - min_millis) / 255;
    auto remap = [](uint16_t millis) { return (millis - min_millis) / divider; };

    remapped_depth.resize(num_pixels);

    std::transform(data.cbegin(), data.cend(), remapped_depth.begin(), remap);

    depth_img = QImage((uchar*) remapped_depth.data(),
                       frame_size.width(),
                       frame_size.height(),
                       QImage::Format_Grayscale8);
    ui->depthLabel->setPixmap(QPixmap::fromImage(depth_img));
}

VideoPlayer::~VideoPlayer()
{
    delete ui;
}
