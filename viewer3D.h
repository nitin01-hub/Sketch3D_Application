#ifndef VIEWER3D_H
#define VIEWER3D_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QVector>
#include <QTimer>
#include "shape.h"

class Viewer3D : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit Viewer3D(QWidget* parent = nullptr);
    void setShapes(const QVector<Shape>& shapes);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    QVector<Shape> m_shapes;
    float          m_rotX = 20.0f;
    float          m_rotY = 30.0f;
    float          m_zoom = -6.0f;
    QPoint         m_lastMouse;
    QTimer*        m_autoRotateTimer;
    float          m_autoAngle = 0.0f;

    void drawShapeExtruded(const Shape& shape);
};

#endif // VIEWER3D_H
