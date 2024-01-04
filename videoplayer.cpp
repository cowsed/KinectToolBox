#include "videoplayer.h"
#include "ui_videoplayer.h"
#include <iostream>

constexpr QSize frame_size(640, 480);

uint8_t default_rgb_image_data[640 * 480 * 3] = { 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF };
uint16_t default_depth_image_data[640 * 480] = { 0xFFFF };

VideoPlayer::VideoPlayer(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VideoPlayer)
    , depth_img((uchar*) default_depth_image_data,
                frame_size.width(),
                frame_size.height(),
                QImage::Format_Grayscale16)
    , rgb_img((uchar*) default_rgb_image_data,
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

    rgb_img = QImage((uchar*)default_rgb_image_data, frame_size.width(), frame_size.height(), QImage::Format_RGB888);
    depth_img = QImage((uchar*)default_depth_image_data, frame_size.width(), frame_size.height(), QImage::Format_Grayscale16);

    ui->rgbLabel->setPixmap(QPixmap::fromImage(rgb_img));
    ui->depthLabel->setPixmap(QPixmap::fromImage(depth_img));
}

void VideoPlayer::set_rgb_data(std::span<uint8_t> data, VideoType typ)
{
    if (!do_updates) {
        return;
    }
    QImage::Format fmt = QImage::Format_RGB888;
    if (typ == VideoType::IR) {
        fmt = QImage::Format_Grayscale8;
    }
    rgb_img = QImage((uchar*)data.data(), frame_size.width(), frame_size.height(), fmt);
    ui->rgbLabel->setPixmap(QPixmap::fromImage(rgb_img));
}
void VideoPlayer::set_depth_data(std::span<uint16_t> data)
{
    if (!do_updates) {
        return;
    }

    const auto min_mm = 300;
    const auto max_mm = 3500;
    const auto divider = (max_mm - min_mm) / 255;
    auto remap = [](uint16_t mm) { return (mm - min_mm) / divider; };

    remapped_depth.resize(640 * 480);
    for (size_t i = 0; i < data.size(); i++) {
        remapped_depth[i] = remap(data[i]);
    }

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
