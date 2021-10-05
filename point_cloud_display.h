#ifndef POINT_CLOUD_DISPLAY_H
#define POINT_CLOUD_DISPLAY_H

#include <QMainWindow>
#include <QColor>
#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include "three_dim_image.h"

class PointCloudDisplay : public QOpenGLWidget, protected QOpenGLFunctions {
public:
    PointCloudDisplay(ThreeDimImage *_images);
    void initializeGL();
    void paintGL();
    void updateContent();

protected slots:
  void resizeEvent(QResizeEvent *event) override;

private:
    ThreeDimImage *images;
};

#endif // POINT_CLOUD_DISPLAY_H
