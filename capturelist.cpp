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

void CaptureList::add_capture(VideoCapture vc)
{
    int id = next_id();
    CapturePreview* prev = new CapturePreview(id, vc, nullptr);
    captures.push_back(prev);
    auto listItem = new QListWidgetItem();
    listItem->setSizeHint(prev->sizeHint());
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

void CaptureList::show_hide()
{
    // auto selected = this->ui->cap
    QList<QListWidgetItem*> selected = this->ui->captureListWidget->selectedItems();
    if (selected.size() == 0) {
        return;
    }

    bool first_is_visible = false;
    auto list = ui->captureListWidget;
    if (CapturePreview* cap = dynamic_cast<CapturePreview*>(
            list->itemWidget(selected[selected.size() - 1]))) {
        first_is_visible = cap->is_shown();
    } else {
        std::cout << "non capture preview widget in capture list " << std::endl;
        return;
    }
    bool set_to = !first_is_visible;
    for (int i = 0; i < selected.size(); i++) {
        if (CapturePreview* cap = dynamic_cast<CapturePreview*>(list->itemWidget(selected[i]))) {
            // first_is_visible = cap->is_shown();
            cap->set_shown(set_to);
        } else {
            std::cout << "non capture preview widget in capture list " << std::endl;
            continue;
        }
    }
}
CaptureList::~CaptureList()
{
    delete ui;
}
