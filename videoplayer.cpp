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
    , color_gradient(256)
{
    ui->setupUi(this);

    ui->depthLabel->setPixmap(QPixmap::fromImage(depth_img));
    ui->rgbLabel->setPixmap(QPixmap::fromImage(rgb_img));
}

void VideoPlayer::set_rgb_data(std::span<uint8_t> data)
{
    rgb_img = QImage((uchar*)data.data(), frame_size.width(), frame_size.height(), QImage::Format_RGB888);
    ui->rgbLabel->setPixmap(QPixmap::fromImage(rgb_img));
}
void VideoPlayer::set_depth_data(std::span<uint16_t> data)
{
    depth_img = QImage((uchar*)data.data(), frame_size.width(), frame_size.height(), QImage::Format_Grayscale16);
    QImage heatmap = depth_img.convertToFormat(QImage::Format_Indexed8);
    heatmap.setColorTable(color_gradient.getColorMap());
    // depth_img.convertToColorSpace(Q)
    ui->depthLabel->setPixmap(QPixmap::fromImage(heatmap));
}

VideoPlayer::~VideoPlayer()
{
    // delete ui->graphicsView->scene();
    delete ui;
}
