#include "capturelist.h"
#include <QPainter>
#include "ui_capturelist.h"
#include <iostream>
#include <utility>

CaptureList::CaptureList(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CaptureList)
{
    ui->setupUi(this);
}
void CaptureList::save_all(const QString &path)
{
    for (auto &cap : captures) {
        QString time = cap->get_time().toString(Qt::DateFormat::ISODateWithMs);
        QString fname = path + "/" + time.replace(' ', '_') + ".pcd";
        std::cout << "saved cloud to " << fname.toStdString() << std::endl;
        cap->points_to_file(fname.toStdString());
    }
}

void CaptureList::add_capture(VideoCapture cap, const PointFilter::Filter &filt)
{
    QDateTime time = QDateTime::currentDateTime();
    int cap_id = next_id();

    auto *prev = new CapturePreview(cap_id, std::move(cap), filt, time, nullptr);

    captures.push_back(prev);
    auto *listItem = new QListWidgetItem();
    listItem->setSizeHint(prev->sizeHint());
    ui->captureListWidget->addItem(listItem);

    listItem->setToolTip("Capture: " + time.toString());
    ui->captureListWidget->setItemWidget(listItem, prev);

    ui->numCaptureLabel->setText(
        QString::fromStdString(std::format("Captures: {}", captures.size())));
    connect(prev, &CapturePreview::visibility_changed, this, &CaptureList::show_hide_one);
}
void CaptureList::request_capture()
{
    emit take_capture();
}

void CaptureList::toggle_recording()
{
    if (recording) {
        ui->recordButton->setText(play_icon);
        ui->captureTimeSpinbox->setDisabled(false);
        ui->captureButton->setDisabled(false);
        recording = false;
        timer.stop();

    } else {
        ui->recordButton->setText(pause_icon);
        ui->captureTimeSpinbox->setDisabled(true);
        ui->captureButton->setDisabled(true);

        recording = true;
        timer.start(ui->captureTimeSpinbox->value());
        timer.callOnTimeout([this]() { emit take_capture(); });
    }
}

int CaptureList::next_id()
{
    id++;
    return id;
}

PointSupplier CaptureList::points_supplier()
{
    return [this]() {
        std::vector<PointCloud::Ptr> clouds;
        for (CapturePreview* cap : captures) {
            if (cap->is_shown()) {
                auto cloud = cap->points();
                clouds.push_back(cloud);
            }
        }
        return clouds;
    };
}

void CaptureList::toggle_show_hide()
{
    auto *list = ui->captureListWidget;
    QList<QListWidgetItem*> selected = list->selectedItems();
    if (selected.empty()) {
        return;
    }

    bool first_is_visible = false;
    if (auto *cap = dynamic_cast<CapturePreview *>(list->itemWidget(list->currentItem()))) {
        first_is_visible = cap->is_shown();
    } else {
        std::cout << "non capture preview widget in capture list " << std::endl;
        return;
    }
    bool set_to = !first_is_visible;
    for (auto *item : selected) {
        if (auto *cap = dynamic_cast<CapturePreview *>(list->itemWidget(item))) {
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
    for (auto *cap : captures) {
        delete cap;
    }

    delete ui;
}
