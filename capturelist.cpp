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
    auto time = QDateTime::currentDateTime();
    int id = next_id();

    CapturePreview* prev = new CapturePreview(id, vc, time, nullptr);

    captures.push_back(prev);
    auto listItem = new QListWidgetItem();
    listItem->setSizeHint(prev->sizeHint());
    ui->captureListWidget->addItem(listItem);
    QString tt = "Capture: ";
    tt += time.toString();

    listItem->setToolTip(tt);
    ui->captureListWidget->setItemWidget(listItem, prev);

    ui->numCaptureLabel->setText(
        QString::fromStdString(std::format("Captures: {}", captures.size())));
    connect(prev, &CapturePreview::visibility_changed, this, &CaptureList::show_hide_one);

    auto fname = prev->get_time().toString();
    fname.replace(' ', '_');
    prev->points_to_file(std::format("capture_{}.pcd", fname.toStdString()));
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

PointSupplier CaptureList::points_supplier()
{
    return [this]() {
        std::vector<std::span<Point>> pts;
        for (CapturePreview* cap : captures) {
            if (cap->is_shown()) {
                pts.push_back(cap->points());
            }
        }
        return pts;
    };
}

void CaptureList::toggle_show_hide()
{
    auto list = ui->captureListWidget;
    QList<QListWidgetItem*> selected = list->selectedItems();
    if (selected.size() == 0) {
        return;
    }

    bool first_is_visible = false;
    if (CapturePreview* cap = dynamic_cast<CapturePreview*>(list->itemWidget(list->currentItem()))) {
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
    emit visible_captures_changed();
}
void CaptureList::show_hide_one()
{
    emit visible_captures_changed();
}

CaptureList::~CaptureList()
{
    delete ui;
}
