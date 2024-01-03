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
  void set_supplier(PointSupplier ps);

  public slots:
  void set_x_angle(int ang);
  void set_y_angle(int ang);
  void set_zoom(int zoom);
  void set_point_size(int size);
  void reset();
  void point_supplier_update();

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
  float point_size = 1;
  int last_rx = 0;
  int last_ry = 0;

  PointSupplier supplier;
  PointSupply points;

  Ui::PointCloudRenderer *ui;
};

#endif // POINTCLOUDRENDERER_H
