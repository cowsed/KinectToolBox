#include "capturepreview.h"
#include "ui_capturepreview.h"
#include <iostream>

CapturePreview::CapturePreview(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::CapturePreview)
{
    ui->setupUi(this);
}

CapturePreview::CapturePreview(int id, VideoCapture cap, QWidget* parent)
    : CapturePreview(parent)
{
    this->id = id;
    this->cap = cap;
    this->ui->idLabel->setText(QString::fromStdString(std::format("{}", id)));
    QImage img((uchar*) cap.rgb.data(), 640, 480, QImage::Format::Format_RGB888);
    this->ui->imagePreview->setPixmap(QPixmap::fromImage(img));
}

void CapturePreview::set_shown(bool s)
{
    Qt::CheckState cs = Qt::Unchecked;
    if (s) {
        cs = Qt::Checked;
    }
    this->ui->checkBox->setCheckState(cs);
}

bool CapturePreview::is_shown()
{
    return this->ui->checkBox->isChecked();
}

CapturePreview::~CapturePreview()
{
    delete ui;
}
