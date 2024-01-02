#include "pointcloudrenderer.h"
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
  QBasicTimer::start(16, Qt::TimerType::PreciseTimer, this);
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

  glPointSize(1.0f);

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
void PointCloudRenderer::unset_device(){
  device = std::nullopt;
}

void PointCloudRenderer::set_rgb_data(std::span<uint8_t> data)
{
  for (size_t i = 0; i < points.size(); i++) {
    points[i].r = data[3 * i];
    points[i].g = data[3 * i + 1];
    points[i].b = data[3 * i + 2];
  }
}
void PointCloudRenderer::set_depth_data(std::span<uint16_t> data)
{
  for (size_t i = 0; i < data.size(); i++) {
    int x = i % 640;
    int y = i / 640;
    uint16_t depth = data[i];
    points[i].pos = MyFreenectDevice::pixel_to_point(x, y, depth);
  }
}

PointCloudRenderer::~PointCloudRenderer()
{
  delete ui;
}
