#ifndef CAPTURELIST_H
#define CAPTURELIST_H

#include <QDataStream>
#include <QJsonObject>
#include <QTimer>
#include <QWidget>
#include "capturepreview.h"
#include "kinect_types.h"
namespace Ui {
class CaptureList;
}

/**
 * @brief The CaptureList class
 * Keeps a list of captures we can later align
 * Represents them in CapturePreview which keeps an image preview, point cloud and visible flag
 */
class CaptureList : public QWidget {
    Q_OBJECT

public:
    const QString play_icon = "▶️";
    const QString pause_icon = "⏸️";
    explicit CaptureList(QWidget *parent = nullptr);
    ~CaptureList();

    int next_id();
    PointSupplier points_supplier();
    void save_all(const QString &path);
signals:
    /**
     * @brief take_capture
     * Request the freenect device that you wan't a capture
     */
    void take_capture();
    /**
     * @brief visible_captures_changed
     * Alert the capture renderer that what they should be drawing has changed
     */
    void visible_captures_changed();

public slots:
    /**
     * @brief add_capture recieve a capture from the freenect device and add it to our list
     * @param vc the capture
     */
    void add_capture(VideoCapture vc, PointFilter::Filter filt);
    /**
     * @brief request_capture tell this widget to tell the freenect device to take a capture
     */
    void request_capture();

    /**
     * @brief toggle_show_hide react to the show hide key being pressed
     */
    void toggle_show_hide();
    void show_hide_one();
    void toggle_recording();

private:
    Ui::CaptureList* ui;
    int id = 0;
    std::vector<CapturePreview*> captures;
    bool recording = false;
    QTimer timer;
};

#endif // CAPTURELIST_H
