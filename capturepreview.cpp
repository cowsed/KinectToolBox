#include "capturepreview.h"
#include "myfreenectdevice.h"
#include "ui_capturepreview.h"
#include <iostream>

CapturePreview::CapturePreview(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::CapturePreview)
{
    ui->setupUi(this);
    connect(ui->checkBox, &QCheckBox::clicked, this, &CapturePreview::checkbox_changed);
}

CapturePreview::CapturePreview(int id, VideoCapture cap, QWidget* parent)
    : CapturePreview(parent)
{
    this->id = id;
    this->cap = cap;
    this->ui->idLabel->setText(QString::fromStdString(std::format("{}", id)));
    QImage img((uchar*) cap.rgb.data(), 640, 480, QImage::Format::Format_RGB888);
    this->ui->imagePreview->setPixmap(QPixmap::fromImage(img));

    pts.resize(640 * 480);

    for (size_t i = 0; i < cap.depth.size(); i++) {
        int x = i % 640;
        int y = i / 640;
        uint16_t depth = cap.depth[i];
        pts[i].pos = MyFreenectDevice::pixel_to_point(x, y, depth);
    }
    for (size_t i = 0; i < pts.size(); i++) {
        pts[i].r = cap.rgb[3 * i];
        pts[i].g = cap.rgb[3 * i + 1];
        pts[i].b = cap.rgb[3 * i + 2];
    }
}
void CapturePreview::checkbox_changed()
{
    emit visibility_changed();
}

int CapturePreview::get_id()
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

std::span<Point> CapturePreview::points()
{
    return std::span(pts);
}

bool CapturePreview::is_shown()
{
    return this->ui->checkBox->isChecked();
}

CapturePreview::~CapturePreview()
{
    delete ui;
}
