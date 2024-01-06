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

static auto get_color(int i, std::span<uint8_t> rgb) -> std::tuple<uint8_t, uint8_t, uint8_t>
{
        auto r = rgb[3 * i];
        auto g = rgb[3 * i + 1];
        auto b = rgb[3 * i + 2];
        return {r, g, b};
}

CapturePreview::CapturePreview(int id,
                               VideoCapture video_capture,
                               PointFilter::Filter filt,
                               QDateTime time,
                               QWidget *parent)
    : CapturePreview(parent)
{
    this->id = id;
    this->time = time;

    thumbnail.resize(4 * 640 * 480);

    PointCloud::Ptr newcloud = std::make_shared<PointCloud>();
    newcloud->reserve(640 * 480);

    // Build thumbnail
    for (size_t i = 0; i < video_capture.depth.size(); i++) {
        int ix = i % 640;
        int iy = i / 640;
        uint16_t depth = video_capture.depth[i];
        auto [r, g, b] = get_color(i, video_capture.rgb);
        auto [x, y, z] = MyFreenectDevice::pixel_to_point(ix, iy, depth);

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
    QImage img((uchar *) thumbnail.data(), 640, 480, QImage::Format::Format_RGBA8888);

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

void CapturePreview::set_shown(bool s)
{
    Qt::CheckState cs = Qt::Unchecked;
    if (s) {
        cs = Qt::Checked;
    }
    this->ui->checkBox->setCheckState(cs);
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

// QDataStream &operator<<(QDataStream &out, const CapturePreview &item)
// {
//     QDataStream::FloatingPointPrecision prev = out.floatingPointPrecision();
//     out.setFloatingPointPrecision(QDataStream::DoublePrecision);
//     QList<Point> pts(item.pts.size());
//     std::copy(item.pts.begin(), item.pts.end(), pts.begin());

//     out << item.get_id() << item.get_time() << item.is_shown() << pts;

//     out.setFloatingPointPrecision(prev);
//     return out;
// }
