#include "capturepreview.h"
#include "myfreenectdevice.h"
#include "pcl/io/pcd_io.h"
#include "ui_capturepreview.h"
#include <iostream>

#include <fstream>

CapturePreview::CapturePreview(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::CapturePreview)
{
    ui->setupUi(this);
    connect(ui->checkBox, &QCheckBox::clicked, this, &CapturePreview::checkbox_changed);
}

CapturePreview::CapturePreview(int id,
                               VideoCapture video_capture,
                               const PointFilter::Filter &filt,
                               QDateTime time,
                               QWidget *parent)
    : CapturePreview(parent)
{
    this->id = id;
    this->time = time;

    thumbnail.resize(rgba_buffer_size);

    PointCloud::Ptr newcloud = std::make_shared<PointCloud>();
    newcloud->reserve(num_pixels);

    // Build thumbnail
    for (size_t i = 0; i < video_capture.pix.size(); i++) {
        int image_x = i % kinect_video_width;
        int image_y = i / kinect_video_width;

        uint16_t depth = video_capture.pix[i].depth;
        auto [r, g, b] = video_capture.pix[i].col;

        auto [x, y, z] = MyFreenectDevice::pixel_to_point(image_x, image_y, depth);

        Point p(x, y, z, r, g, b);
        thumbnail[4 * i] = r;
        thumbnail[4 * i + 1] = g;
        thumbnail[4 * i + 2] = b;

        if (filt(p) && depth != 0) {
            thumbnail[4 * i + 3] = 0xff;
            newcloud->push_back(p);
        } else {
            thumbnail[4 * i + 3] = 0x00;
        }
    }
    pts = newcloud;

    this->ui->idLabel->setText(QString::fromStdString(std::format("{}", id)));
    QImage img((uchar *) thumbnail.data(),
               kinect_video_width,
               kinect_video_height,
               QImage::Format::Format_RGBA8888);

    this->ui->imagePreview->setPixmap(QPixmap::fromImage(img));
}

void CapturePreview::points_to_file(const std::string &fname)
{
    pcl::PCDWriter pw;
    pw.write<Point>(fname, *pts);
}

void CapturePreview::checkbox_changed()
{
    emit visibility_changed();
}

int CapturePreview::get_id() const
{
    return id;
}

void CapturePreview::set_shown(bool shown)
{
    Qt::CheckState checked = Qt::Unchecked;
    if (shown) {
        checked = Qt::Checked;
    }
    this->ui->checkBox->setCheckState(checked);
}

PointCloud::Ptr CapturePreview::points()
{
    return pts;
}

bool CapturePreview::is_shown() const
{
    return this->ui->checkBox->isChecked();
}

QDateTime CapturePreview::get_time() const
{
    return time;
}

CapturePreview::~CapturePreview()
{
    delete ui;
}
