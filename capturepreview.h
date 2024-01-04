#ifndef CAPTUREPREVIEW_H
#define CAPTUREPREVIEW_H

#include <QList>
#include <QWidget>
#include "kinect_types.h"
#include "qdatetime.h"

namespace Ui {
class CapturePreview;
}

class CapturePreview : public QWidget {
    Q_OBJECT

public:
    friend QDataStream &operator<<(QDataStream &out, const CapturePreview &item);
    struct Capture {
        DepthPointCapture pts;
        int id;
    };

    explicit CapturePreview(QWidget* parent = nullptr);
    CapturePreview(int id, VideoCapture cap, QDateTime time, QWidget *parent = nullptr);
    ~CapturePreview();
    bool is_shown() const;
    std::span<Point> points();
    int get_id() const;

    QDateTime get_time() const;

    void points_to_file(const std::string &fname);

public slots:
    void set_shown(bool s);
    void checkbox_changed();
signals:
    void visibility_changed();

private:
    Ui::CapturePreview* ui;
    VideoCapture cap;
    PointCapture pts;
    QDateTime time;
    int id;
};

QDataStream &operator<<(QDataStream &out, const CapturePreview &item);

#endif // CAPTUREPREVIEW_H
