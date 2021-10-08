#ifndef POINT_CLOUD_DISPLAY_H
#define POINT_CLOUD_DISPLAY_H

#include <QMainWindow>
#include <QColor>
#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QVector>
#include <cmath>
#include "three_dim_image.h"

class PointCloudDisplay : public QOpenGLWidget, protected QOpenGLFunctions {
public:
    PointCloudDisplay(ThreeDimImage *_images);
    void initializeGL();
    void paintGL();
private:
    void lookAt(QVector3D eye, QVector3D target, QVector3D up);
    ThreeDimImage *images;
};

#endif // POINT_CLOUD_DISPLAY_H
