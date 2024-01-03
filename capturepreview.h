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
    std::span<Point> points();
    int get_id();
public slots:
    void set_shown(bool s);
    void checkbox_changed();
signals:
    void visibility_changed();

private:
    Ui::CapturePreview* ui;
    VideoCapture cap;
    std::vector<Point> pts;
    int id;
};

#endif // CAPTUREPREVIEW_H
