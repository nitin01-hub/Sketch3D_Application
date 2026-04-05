#include "shape.h"
#include <cmath>
#include <QPolygonF>

Shape::Shape(ShapeType type, QPointF origin, QColor color)
    : m_type(type), m_origin(origin), m_color(color)
{
    if (type == ShapeType::Square) {
        m_height = m_width; // Force square aspect
    }
}

QString Shape::label() const {
    switch (m_type) {
    case ShapeType::Triangle:  return "Triangle";
    case ShapeType::Rectangle: return "Rectangle";
    case ShapeType::Square:    return "Square";
    }
    return "Unknown";
}

QVector<QPointF> Shape::getVertices2D() const {
    float w = m_width  * m_scaleX;
    float h = (m_type == ShapeType::Square ? w : m_height * m_scaleY);
    float x = m_origin.x();
    float y = m_origin.y();

    QVector<QPointF> verts;
    switch (m_type) {
    case ShapeType::Triangle:
        verts << QPointF(x,         y - h)
              << QPointF(x - w / 2, y)
              << QPointF(x + w / 2, y);
        break;
    case ShapeType::Rectangle:
    case ShapeType::Square:
        verts << QPointF(x,     y)
              << QPointF(x + w, y)
              << QPointF(x + w, y + h)
              << QPointF(x,     y + h);
        break;
    }
    return verts;
}

bool Shape::contains(QPointF point) const {
    QVector<QPointF> verts = getVertices2D();
    QPolygonF poly;
    for (auto& v : verts) poly << v;
    return poly.containsPoint(point, Qt::OddEvenFill);
}
