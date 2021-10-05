#include "point_cloud_display.h"

PointCloudDisplay::PointCloudDisplay(ThreeDimImage *_images) {
  images = _images;
}

void PointCloudDisplay::initializeGL() {
  initializeOpenGLFunctions();
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
}

void PointCloudDisplay::paintGL() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  // TODO: Add camera n stuff

  glBegin(GL_POINTS);
  std::vector<Voxel> voxels = images->GetVoxels();
  for (unsigned long i = 0; i < voxels.size(); i++) {
    glColor4f(voxels[i].color / 255.f, voxels[i].color / 255.f, voxels[i].color / 255.f, 1);
    glVertex3d(voxels[i].x, voxels[i].y, voxels[i].z);
  }
  glEnd();
}

// rescaling
void PointCloudDisplay::updateContent() {
    std::cout << "h, w: " << this->width() << ", " << this->height() << std::endl;
    std::cout << "x, y: " << this->x() << ", " << this->y() << std::endl;
    int size = std::min(this->width(), this->height());
    this->resize(size, size);
    //this->move(size/2, size/2);
}

// rescaling
void PointCloudDisplay::resizeEvent(QResizeEvent *event) {
  (void)event;
  updateContent();
}
