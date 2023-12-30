#ifndef POINTCLOUDRENDERER_H
#define POINTCLOUDRENDERER_H

#include "qopenglwidget.h"
#include <QWidget>

namespace Ui {
class PointCloudRenderer;
}

class PointCloudRenderer : public QOpenGLWidget
{
  Q_OBJECT

public:
  explicit PointCloudRenderer(QWidget *parent = nullptr);
  ~PointCloudRenderer();


protected:
  void initializeGL();
  void resizeGL(int w, int h);
  void paintGL();


private:
  Ui::PointCloudRenderer *ui;
};

#endif // POINTCLOUDRENDERER_H
