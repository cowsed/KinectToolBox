#include "capturelist.h"
#include "ui_capturelist.h"
#include <QPainter>
#include <iostream>

CaptureList::CaptureList(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::CaptureList)
{
    ui->setupUi(this);
}

void CaptureList::add_capture(PointCapture pc)
{
    int id = next_id();
    CapturePreview* prev = new CapturePreview(id, pc, nullptr);
    captures.push_back(prev);
    auto listItem = new QListWidgetItem();
    ui->captureListWidget->addItem(listItem);
    ui->captureListWidget->setItemWidget(listItem, prev);
    ui->numCaptureLabel->setText(QString::fromStdString(std::format("Captures: {}", captures.size())));
}
void CaptureList::request_capture()
{
    emit take_capture();
}

int CaptureList::next_id()
{
    id++;
    return id;
}
CaptureList::~CaptureList()
{
    delete ui;
}
