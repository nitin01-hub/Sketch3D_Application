#include "viewer3D.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <QtMath>
#include <QOpenGLFunctions>

#ifdef Q_OS_MAC
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

Viewer3D::Viewer3D(QWidget* parent) : QOpenGLWidget(parent) {
    setMinimumSize(400, 400);
    m_autoRotateTimer = new QTimer(this);
    connect(m_autoRotateTimer, &QTimer::timeout, this, [this]() {
        m_autoAngle += 0.5f;
        update();
    });
    m_autoRotateTimer->start(16);
}

void Viewer3D::setShapes(const QVector<Shape>& shapes) {
    m_shapes = shapes;
    update();
}

void Viewer3D::initializeGL() {
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);

    GLfloat lightPos[] = { 3.0f, 5.0f, 5.0f, 1.0f };
    GLfloat ambient[]  = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat diffuse[]  = { 0.9f, 0.9f, 0.9f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);

    glClearColor(0.12f, 0.12f, 0.18f, 1.0f);
}

void Viewer3D::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void Viewer3D::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Camera
    glTranslatef(0.0f, 0.0f, m_zoom);
    glRotatef(m_rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(m_rotY + m_autoAngle * 0.3f, 0.0f, 1.0f, 0.0f);

    // Draw grid floor
    glDisable(GL_LIGHTING);
    glColor3f(0.25f, 0.25f, 0.35f);
    glBegin(GL_LINES);
    for (int i = -5; i <= 5; ++i) {
        glVertex3f(i, 0, -5); glVertex3f(i, 0,  5);
        glVertex3f(-5, 0, i); glVertex3f( 5, 0, i);
    }
    glEnd();
    glEnable(GL_LIGHTING);

    // Spread shapes evenly
    int n = m_shapes.size();
    for (int i = 0; i < n; ++i) {
        float angle  = (n > 1) ? (360.0f / n * i) : 0.0f;
        float radius = (n > 1) ? 2.0f : 0.0f;
        float posX   = radius * qCos(qDegreesToRadians(angle));
        float posZ   = radius * qSin(qDegreesToRadians(angle));

        glPushMatrix();
        glTranslatef(posX, 0.0f, posZ);
        drawShapeExtruded(m_shapes[i]);
        glPopMatrix();
    }
}

void Viewer3D::drawShapeExtruded(const Shape& shape) {
    QVector<QPointF> verts2D = shape.getVertices2D();
    if (verts2D.isEmpty()) return;

    // Normalize: center the shape at origin and scale to fit 3D view
    QPointF centroid(0, 0);
    for (auto& v : verts2D) centroid += v;
    centroid /= verts2D.size();

    float scale = 0.008f;
    float depth = 0.5f;

    QColor c = shape.color();
    float r = c.redF(), g = c.greenF(), b = c.blueF();

    auto vx = [&](int i) { return (float)(verts2D[i].x() - centroid.x()) * scale; };
    auto vy = [&](int i) { return -(float)(verts2D[i].y() - centroid.y()) * scale; };

    int n = verts2D.size();

    // Top face
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    for (int i = 0; i < n; ++i)
        glVertex3f(vx(i), depth, vy(i));
    glEnd();

    // Bottom face (darker)
    glColor3f(r * 0.5f, g * 0.5f, b * 0.5f);
    glBegin(GL_POLYGON);
    for (int i = n - 1; i >= 0; --i)
        glVertex3f(vx(i), 0.0f, vy(i));
    glEnd();

    // Side faces
    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        // Normal for lighting
        float nx = vy(j) - vy(i);
        float nz = -(vx(j) - vx(i));
        glNormal3f(nx, 0.0f, nz);
        glColor3f(r * 0.75f, g * 0.75f, b * 0.75f);
        glBegin(GL_QUADS);
        glVertex3f(vx(i), 0.0f,  vy(i));
        glVertex3f(vx(j), 0.0f,  vy(j));
        glVertex3f(vx(j), depth, vy(j));
        glVertex3f(vx(i), depth, vy(i));
        glEnd();
    }

    // Edges
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < n; ++i)
        glVertex3f(vx(i), depth + 0.001f, vy(i));
    glEnd();
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < n; ++i)
        glVertex3f(vx(i), -0.001f, vy(i));
    glEnd();
    glEnable(GL_LIGHTING);
}

void Viewer3D::mousePressEvent(QMouseEvent* event) {
    m_lastMouse = event->pos();
    m_autoRotateTimer->stop();
}

void Viewer3D::mouseMoveEvent(QMouseEvent* event) {
    QPoint delta = event->pos() - m_lastMouse;
    m_rotY += delta.x() * 0.5f;
    m_rotX += delta.y() * 0.5f;
    m_lastMouse = event->pos();
    update();
}

void Viewer3D::wheelEvent(QWheelEvent* event) {
    m_zoom += event->angleDelta().y() * 0.005f;
    m_zoom  = qBound(-20.0f, m_zoom, -1.5f);
    update();
}
