#include "point_cloud_display.h"

PointCloudDisplay::PointCloudDisplay(ThreeDimImage *_images) {
  images = _images;
}

void PointCloudDisplay::initializeGL() {
  initializeOpenGLFunctions();
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  //glEnable(GL_DEPTH_TEST);
  //glDepthFunc(GL_LESS);

  glMatrixMode(GL_PROJECTION);
  glOrtho(-1, 1, -1, 1, 0, 10);
}

void PointCloudDisplay::paintGL() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  glMatrixMode(GL_MODELVIEW);
  glTranslatef(0.0f, 2.0f, 0.0f);
  glRotatef(180.0f, 180.0f, 0.0f, 180.0f);

  glBegin(GL_POINTS);
  std::vector<Voxel> voxels = images->GetVoxels();
  for (unsigned long i = 0; i < voxels.size(); i++) {
    glColor4f(voxels[i].color, voxels[i].color, voxels[i].color, alpha);
    glVertex3d(voxels[i].x, voxels[i].y, voxels[i].z);
  }
  glEnd();
}

void PointCloudDisplay::setAlpha(float new_alpha) {
  this->alpha = new_alpha;
}

double PointCloudDisplay::getAlpha() {
  return this->alpha;
}

// rescaling
void PointCloudDisplay::updateContent() {
    int size = std::min(this->width(), this->height());
    this->resize(size, size);
}

// rescaling
void PointCloudDisplay::resizeEvent(QResizeEvent *event) {
  (void)event;
  updateContent();
}
