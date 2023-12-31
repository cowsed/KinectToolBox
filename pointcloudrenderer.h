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

protected:
  void initializeGL();
  void resizeGL(int w, int h);
  void paintGL();
  void timerEvent(QTimerEvent *event);

private:
  Ui::PointCloudRenderer *ui;
  std::optional<MyFreenectDevice*> device;
};

#endif // POINTCLOUDRENDERER_H
