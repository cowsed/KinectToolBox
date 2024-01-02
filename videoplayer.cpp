#include "videoplayer.h"
#include "ui_videoplayer.h"

constexpr QSize frame_size(640, 480);

uint8_t rgb_image_data[640 * 480 * 3] = { 0xFF };
uint16_t depth_image_data[640 * 480] = { 0xFFFF };

VideoPlayer::VideoPlayer(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VideoPlayer)
    , depth_img((uchar*)depth_image_data, frame_size.width(), frame_size.height(), QImage::Format_Grayscale16)
    , rgb_img((uchar*)rgb_image_data, frame_size.width(), frame_size.height(), QImage::Format_RGB888)
{
    ui->setupUi(this);

    ui->depthLabel->setPixmap(QPixmap::fromImage(depth_img));
    ui->rgbLabel->setPixmap(QPixmap::fromImage(rgb_img));
}

void VideoPlayer::set_rgb_data(std::span<uint8_t> data, VideoType typ)
{
    QImage::Format fmt = QImage::Format_RGB888;
    if (typ == VideoType::IR) {
        fmt = QImage::Format_Grayscale8;
    }
    rgb_img = QImage((uchar*)data.data(), frame_size.width(), frame_size.height(), fmt);
    ui->rgbLabel->setPixmap(QPixmap::fromImage(rgb_img));
}
void VideoPlayer::set_depth_data(std::span<uint16_t> data)
{
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
    heatmap.setColorTable(ct);
    ui->depthLabel->setPixmap(QPixmap::fromImage(heatmap));
}

VideoPlayer::~VideoPlayer()
{
    // delete ui->graphicsView->scene();
    delete ui;
}
