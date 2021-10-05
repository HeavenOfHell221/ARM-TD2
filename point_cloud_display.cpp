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
