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
    , points(640 * 480, { 0, 0, 0, 0, 0, 0 })
{
  ui->setupUi(this);
  ui->numPointsLabel->setText("Disconnected");
  QBasicTimer::start(16, Qt::TimerType::PreciseTimer, this);
}

void PointCloudRenderer::reset()
{
  device = std::nullopt;
  ui->numPointsLabel->setText("Disconnected");
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
  if (!device.has_value()){
    glClearColor(1.0,0,0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return;
  }
  bool color = true;


  glClearColor(0.45f, 0.45f, 0.45f, 0.0f);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPointSize(point_size);

  glBegin(GL_POINTS);

  if (!color){glColor3ub(255, 255, 255);}

  for (int i = 0; i < points.size(); ++i) {
    auto p = points[i];

    if (color){
        glColor3ub(points[i].r, points[i].g, points[i].b);
    }

    glVertex3f(p.pos.x, p.pos.y, p.pos.z);
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
  // glRotated(anglex, 0, 0, 0);
  gluLookAt(-7 * anglex, -7 * angley, -1000.0, 0.0, 0.0, 2000.0, 0.0, -1.0,
      0.0);
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

void PointCloudRenderer::set_device(MyFreenectDevice *dev){
  device = dev;
}

void PointCloudRenderer::set_rgb_data(std::span<uint8_t> data, VideoType typ)
{

  if (!device.has_value()) {
    return;
  }
  for (size_t i = 0; i < points.size(); i++) {
    if (typ == VideoType::RGB) {
        points[i].r = data[3 * i];
        points[i].g = data[3 * i + 1];
        points[i].b = data[3 * i + 2];
    } else {
        points[i].r = data[i];
        points[i].g = data[i];
        points[i].b = data[i];
    }
  }
  ui->numPointsLabel->setText(QString::fromStdString(std::format("{} points", points.size())));
}

void PointCloudRenderer::set_depth_data(std::span<uint16_t> data)
{

  if (!device.has_value()) {
    return;
  }

  for (size_t i = 0; i < data.size(); i++) {
    int x = i % 640;
    int y = i / 640;
    uint16_t depth = data[i];
    points[i].pos = MyFreenectDevice::pixel_to_point(x, y, depth);
  }
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
