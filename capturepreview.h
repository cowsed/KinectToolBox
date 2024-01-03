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
    CapturePreview(int id, VideoCapture cap, QWidget* parent = nullptr);
    ~CapturePreview();
    bool is_shown();
public slots:
    void set_shown(bool s);

private:
    Ui::CapturePreview* ui;
    VideoCapture cap;
    int id;
};

#endif // CAPTUREPREVIEW_H
