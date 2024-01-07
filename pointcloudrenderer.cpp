#include "pointcloudrenderer.h"
#include "qevent.h"
#include "ui_pointcloudrenderer.h"

#include <GL/glu.h>
#include <GL/gl.h>
#include <iostream>
PointCloudRenderer::PointCloudRenderer(QWidget* parent)
    : QOpenGLWidget(parent)
    , QBasicTimer()
    , ui(new Ui::PointCloudRenderer)
    , points({})
{
  ui->setupUi(this);
  ui->numPointsLabel->setText("0 Points");
  QBasicTimer::start(16, Qt::TimerType::PreciseTimer, this);
}

void PointCloudRenderer::reset()
{
  ui->numPointsLabel->setText("0 Points");
}
void PointCloudRenderer::timerEvent(QTimerEvent *event){
  paintGL();
}
void PointCloudRenderer::initializeGL()
{
  glClearColor(0.45f, 0.45f, 0.45f, 0.0f);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.0f);

  glMatrixMode(GL_PROJECTION);
  gluPerspective(50.0, 1.0, 900.0, 11000.0);

}

void PointCloudRenderer::set_x_angle(int ang){
  anglex = ang;
}
void PointCloudRenderer::set_y_angle(int ang){
  angley = ang;
}
void PointCloudRenderer::set_zoom(int newzoom)
{
  zoom = newzoom / 40.0;
}
void PointCloudRenderer::set_point_size(int size)
{
  point_size = size;
}

void PointCloudRenderer::paintGL()
{
  bool color = true;


  glClearColor(0.45f, 0.45f, 0.45f, 0.0f);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPointSize(point_size);

  glBegin(GL_POINTS);

  if (!color){glColor3ub(255, 255, 255);}

  for (auto &some_points : points) {
      for (int i = 0; i < some_points->size(); ++i) {
          auto &p = (*some_points)[i];

          if (color) {
              glColor3ub(p.r, p.g, p.b);
          }

          glVertex3f(p.x, p.y, p.z);
      }
  }

  glEnd();

  // Draw the world coordinate frame
  const float len = 50;
  glLineWidth(2.0f);
  glBegin(GL_LINES);
  glColor3ub(255, 0, 0); // X-axis
  glVertex3f(0, 0, 0);
  glVertex3f(len, 0, 0);
  glColor3ub(0, 255, 0); // Y-axis
  glVertex3f(0, 0, 0);
  glVertex3f(0, len, 0);
  glColor3ub(0, 0, 255); // Z-axis
  glVertex3f(0, 0, 0);
  glVertex3f(0, 0, len);
  glEnd();

  // Place the camera
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glScalef(zoom, zoom, 1);
  const float scaler = 7;
  const vec3 center = {0.0, 0.0, 2000.0};
  const vec3 up_vec = {0.0, 1.0, 0.0};
  const vec3 eye = {-scaler * anglex, -scaler * angley, -1000.0};

  gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, up_vec.x, up_vec.y, up_vec.z);
  this->update();
}

void PointCloudRenderer::resizeGL(int width, int height)
{
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  const float fovy = 50.0;
  const float near = 900.0;
  const float far = 11000.0;
  gluPerspective(fovy, (float) width / (float) height, near, far);

  glMatrixMode(GL_MODELVIEW);
}

void PointCloudRenderer::set_supplier(PointSupplier supplier)
{
  this->supplier = std::move(supplier);
}

void PointCloudRenderer::point_supplier_update()
{
  points = supplier();
  size_t num = 0;
  for (auto &point_cloud : points) {
      num += point_cloud->size();
  }
  ui->numPointsLabel->setText(QString::fromStdString(std::format("{} Points", num)));
}


void PointCloudRenderer::wheelEvent(QWheelEvent* event)
{
  constexpr float zoom_scaler = 1.0 / 800.0;
  constexpr float min_zoom = 0.05;
  const float amt = (float) event->pixelDelta().y();
  zoom += amt * zoom_scaler;
  if (zoom <= 0) {
      zoom = min_zoom;
  }
}
void PointCloudRenderer::mousePressEvent(QMouseEvent* event)
{

  if (event->buttons().testFlag(Qt::MouseButton::MiddleButton)) {
    anglex = 0;
    angley = 0;
    zoom = 1;
  }
}

void PointCloudRenderer::mouseMoveEvent(QMouseEvent* event)
{
  int rel_x = event->pos().rx();
  int rel_y = event->pos().ry();
  int delta_x = rel_x - last_rx;
  int delta_y = rel_y - last_ry;
  if (event->buttons().testFlag(Qt::MouseButton::LeftButton)) {
    const float scaler = 0.5F;
    anglex -= ((float) delta_x * scaler);
    angley -= ((float) delta_y * scaler);
  }
  last_rx = rel_x;
  last_ry = rel_y;
}

PointCloudRenderer::~PointCloudRenderer()
{
  delete ui;
}
