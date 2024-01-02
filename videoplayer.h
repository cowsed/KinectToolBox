#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include "kinect_types.h"
#include <QPixmap>
#include <QWidget>

namespace Ui {
class VideoPlayer;
}

class VideoPlayer : public QWidget {
    Q_OBJECT

public:
    explicit VideoPlayer(QWidget* parent = nullptr);
    ~VideoPlayer();

public slots:
    void set_rgb_data(std::span<uint8_t> data, VideoType typ);
    void set_depth_data(std::span<uint16_t> data);

    void start();
    void reset();

private:
    bool do_updates;
    Ui::VideoPlayer* ui;
    QImage depth_img;
    QImage rgb_img;
};

#endif // VIDEOPLAYER_H
