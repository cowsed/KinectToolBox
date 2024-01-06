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

  for (auto some_points : points) {
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
  glLineWidth(2.0f);
  glBegin(GL_LINES);
  glColor3ub(255, 0, 0); // X-axis
  glVertex3f(0, 0, 0);
  glVertex3f(50, 0, 0);
  glColor3ub(0, 255, 0); // Y-axis
  glVertex3f(0, 0, 0);
  glVertex3f(0, 50, 0);
  glColor3ub(0, 0, 255); // Z-axis
  glVertex3f(0, 0, 0);
  glVertex3f(0, 0, 50);
  glEnd();

  // Place the camera
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glScalef(zoom, zoom, 1);
  gluLookAt(-7 * anglex, -7 * angley, -1000.0, 0.0, 0.0, 2000.0, 0.0, -1.0, 0.0);
  this->update();
}

void PointCloudRenderer::resizeGL(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(50.0, (float)w / h, 900.0, 11000.0);

  glMatrixMode(GL_MODELVIEW);
}

void PointCloudRenderer::set_supplier(PointSupplier ps)
{
  supplier = ps;
}

void PointCloudRenderer::point_supplier_update()
{
  points = supplier();
  size_t num = 0;
  for (auto &v : points) {
      num += v->size();
  }
  ui->numPointsLabel->setText(QString::fromStdString(std::format("{} Points", num)));
}


void PointCloudRenderer::wheelEvent(QWheelEvent* event)
{

  float amt = event->pixelDelta().y();
  zoom += amt / 800;
  if (zoom <= 0) {
    zoom = 0.05;
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
  int rx = event->pos().rx();
  int ry = event->pos().ry();
  int dx = rx - last_rx;
  int dy = ry - last_ry;
  if (event->buttons().testFlag(Qt::MouseButton::LeftButton)) {
    anglex += (dx * 0.5);
    angley += (dy * 0.5);
  }
  last_rx = rx;
  last_ry = ry;
}

PointCloudRenderer::~PointCloudRenderer()
{
  delete ui;
}
