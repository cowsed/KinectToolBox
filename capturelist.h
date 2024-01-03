#ifndef CAPTURELIST_H
#define CAPTURELIST_H

#include "capturepreview.h"
#include "kinect_types.h"
#include "qlistview.h"
#include <QWidget>

namespace Ui {
class CaptureList;
}

class CaptureList : public QWidget {
    Q_OBJECT

public:
    explicit CaptureList(QWidget* parent = nullptr);
    ~CaptureList();
    int next_id();

signals:
    void take_capture();

public slots:
    void add_capture(VideoCapture pc);
    void request_capture();
    void show_hide();

private:
    Ui::CaptureList* ui;
    int id = 0;
    std::vector<CapturePreview*> captures;
};

#endif // CAPTURELIST_H
