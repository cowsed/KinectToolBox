#include "capturepreview.h"
#include "ui_capturepreview.h"

CapturePreview::CapturePreview(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::CapturePreview)
{
    ui->setupUi(this);
}

CapturePreview::CapturePreview(int id, PointCapture cap, QWidget* parent)
    : CapturePreview(parent)
{
    this->id = id;
    this->cap = cap;
}

CapturePreview::~CapturePreview()
{
    delete ui;
}
