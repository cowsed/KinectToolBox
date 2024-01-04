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

    depth_img = QImage((uchar*)data.data(), frame_size.width(), frame_size.height(), QImage::Format_Grayscale16);
    QImage heatmap = depth_img.convertToFormat(QImage::Format_Indexed8);

    QList<QRgb> ct = {};
    for (int i = 0; i < 255; i++) {
        auto f = 8;
        auto v = f * i;
        if (v > 255) {
            v = 255;
        } else if (v < 0) {
            v = 0;
        }
        ct.append(qRgb(v, v, v));
    }
    ui->depthLabel->setPixmap(QPixmap::fromImage(heatmap));
}

VideoPlayer::~VideoPlayer()
{
    // delete ui->graphicsView->scene();
    delete ui;
}
