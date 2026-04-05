#ifndef SHAPE_H
#define SHAPE_H

#include <QPointF>
#include <QColor>
#include <QString>
#include <QVector>

enum class ShapeType {
    Triangle,
    Rectangle,
    Square
};

class Shape {
public:
    Shape(ShapeType type, QPointF origin, QColor color = Qt::blue);

    ShapeType type() const { return m_type; }
    QPointF origin() const { return m_origin; }
    QColor color() const { return m_color; }
    float scaleX() const { return m_scaleX; }
    float scaleY() const { return m_scaleY; }
    float width() const { return m_width; }
    float height() const { return m_height; }
    bool isSelected() const { return m_selected; }
    QString label() const;

    void setOrigin(QPointF o) { m_origin = o; }
    void setColor(QColor c) { m_color = c; }
    void setScale(float sx, float sy) { m_scaleX = sx; m_scaleY = sy; }
    void setWidth(float w) { m_width = w; }
    void setHeight(float h) { m_height = h; }
    void setSelected(bool s) { m_selected = s; }

    QVector<QPointF> getVertices2D() const;
    bool contains(QPointF point) const;

private:
    ShapeType m_type;
    QPointF   m_origin;
    QColor    m_color;
    float     m_scaleX = 1.0f;
    float     m_scaleY = 1.0f;
    float     m_width  = 100.0f;
    float     m_height = 100.0f;
    bool      m_selected = false;
};

#endif // SHAPE_H
