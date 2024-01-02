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
  void set_rgb_data(std::span<uint8_t>);
  void set_depth_data(std::span<uint16_t>);

  protected:
  void initializeGL();
  void resizeGL(int w, int h);
  void paintGL();
  void timerEvent(QTimerEvent *event);

private:
  float angley = 0;
  float anglex = 0;
  float zoom = 1;
  struct RGBPoint {
      vec3 pos;
      uint8_t r, g, b;
  };

  std::vector<RGBPoint> points;

  Ui::PointCloudRenderer *ui;
  std::optional<MyFreenectDevice*> device;
};

#endif // POINTCLOUDRENDERER_H
