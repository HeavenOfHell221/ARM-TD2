#ifndef POINT_CLOUD_DISPLAY_H
#define POINT_CLOUD_DISPLAY_H

#include <QMainWindow>
#include <QOpenGLWidget>

class PointCloudDisplay : public QOpenGLWidget {
public:
    PointCloudDisplay();
    void initializeGL();
    void paintGL();
};

#endif // POINT_CLOUD_DISPLAY_H
