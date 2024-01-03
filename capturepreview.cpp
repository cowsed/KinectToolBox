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
