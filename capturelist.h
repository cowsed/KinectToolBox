#ifndef CAPTURELIST_H
#define CAPTURELIST_H

#include "capturepreview.h"
#include "kinect_types.h"
#include "qlistview.h"
#include <QWidget>

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
    explicit CaptureList(QWidget* parent = nullptr);
    ~CaptureList();

    int next_id();
    PointSupplier points_supplier();

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
    void add_capture(VideoCapture vc);
    /**
     * @brief request_capture tell this widget to tell the freenect device to take a capture
     */
    void request_capture();

    /**
     * @brief toggle_show_hide react to the show hide key being pressed
     */
    void toggle_show_hide();
    void show_hide_one();

private:
    Ui::CaptureList* ui;
    int id = 0;
    std::vector<CapturePreview*> captures;
};

#endif // CAPTURELIST_H
