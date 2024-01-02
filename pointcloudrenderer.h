#ifndef POINTCLOUDRENDERER_H
#define POINTCLOUDRENDERER_H

#include "myfreenectdevice.h"
#include "qopenglwidget.h"
#include <QWidget>
#include<QTimer>

namespace Ui {
class PointCloudRenderer;
}

class PointCloudRenderer : public QOpenGLWidget, public QBasicTimer
{
  Q_OBJECT

public:
  explicit PointCloudRenderer(QWidget *parent = nullptr);
  ~PointCloudRenderer();
  void set_device(MyFreenectDevice *dev);
  void unset_device();

public slots:
  void set_x_angle(int ang);
  void set_y_angle(int ang);
  void set_zoom(int zoom);
  void set_rgb_data(std::span<uint8_t>, VideoType typ);
  void set_depth_data(std::span<uint16_t>);

  protected:
  void initializeGL();
  void resizeGL(int w, int h);
  void paintGL();
  void timerEvent(QTimerEvent *event);
  void mouseMoveEvent(QMouseEvent* event);
  void mousePressEvent(QMouseEvent* event);
  void wheelEvent(QWheelEvent* event);

  private:
  float angley = 0;
  float anglex = 0;
  float zoom = 1;
  struct RGBPoint {
      vec3 pos;
      uint8_t r, g, b;
  };
  int last_rx = 0;
  int last_ry = 0;

  std::vector<RGBPoint> points;

  Ui::PointCloudRenderer *ui;
  std::optional<MyFreenectDevice*> device;
};

#endif // POINTCLOUDRENDERER_H
