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
    void updateContent();
    void setAlpha(float new_alpha);
    double getAlpha();

protected slots:
  void resizeEvent(QResizeEvent *event) override;

private:
    ThreeDimImage *images;
    double alpha = 0.5;
};

#endif // POINT_CLOUD_DISPLAY_H
