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

CapturePreview::CapturePreview(
    int id, VideoCapture cap, PointCloud::Ptr pc, QDateTime time, QWidget *parent)
    : CapturePreview(parent)
{
    this->id = id;
    this->cap = cap;
    this->time = time;

    this->ui->idLabel->setText(QString::fromStdString(std::format("{}", id)));
    QImage img((uchar*) cap.rgb.data(), 640, 480, QImage::Format::Format_RGB888);
    this->ui->imagePreview->setPixmap(QPixmap::fromImage(img));

    pts = pc;
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
