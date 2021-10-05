#include "point_cloud_display.h"

PointCloudDisplay::PointCloudDisplay(ThreeDimImage *_images) {
    images = _images;
}

void PointCloudDisplay::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void PointCloudDisplay::paintGL() {
    // TODO: Draw voxel
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
