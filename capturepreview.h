#ifndef CAPTUREPREVIEW_H
#define CAPTUREPREVIEW_H

#include "kinect_types.h"
#include <QWidget>

namespace Ui {
class CapturePreview;
}

class CapturePreview : public QWidget {
    Q_OBJECT

public:
    struct Capture {
        PointCapture pts;
        int id;
    };

    explicit CapturePreview(QWidget* parent = nullptr);
    CapturePreview(int id, PointCapture cap, QWidget* parent = nullptr);
    ~CapturePreview();

private:
    Ui::CapturePreview* ui;
    PointCapture cap;
    int id;
};

#endif // CAPTUREPREVIEW_H
