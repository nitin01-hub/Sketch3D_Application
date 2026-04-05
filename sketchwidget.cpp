#include "sketchwidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPolygonF>
#include <QRandomGenerator>

SketchWidget::SketchWidget(QWidget* parent) : QWidget(parent) {
    setMinimumSize(600, 500);
    setStyleSheet("background-color: #1e1e2e;");
    setCursor(Qt::ArrowCursor);
}

void SketchWidget::setTool(SketchTool tool) {
    m_tool = tool;
    if (tool != SketchTool::Select) {
        // Deselect all
        for (auto& s : m_shapes) s.setSelected(false);
        m_selectedIndex = -1;
        setCursor(Qt::CrossCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
    update();
}

void SketchWidget::clearAll() {
    m_shapes.clear();
    m_selectedIndex = -1;
    update();
    emit shapesChanged();
}

void SketchWidget::deleteSelected() {
    if (m_selectedIndex >= 0 && m_selectedIndex < m_shapes.size()) {
        m_shapes.remove(m_selectedIndex);
        m_selectedIndex = -1;
        update();
        emit shapesChanged();
    }
}

void SketchWidget::setSelectedColor(QColor color) {
    if (m_selectedIndex >= 0)
        m_shapes[m_selectedIndex].setColor(color);
    update();
}

void SketchWidget::setSelectedScaleX(double sx) {
    if (m_selectedIndex >= 0) {
        auto& s = m_shapes[m_selectedIndex];
        s.setScale((float)sx, s.scaleY());
        update();
        emit shapesChanged();
    }
}

void SketchWidget::setSelectedScaleY(double sy) {
    if (m_selectedIndex >= 0) {
        auto& s = m_shapes[m_selectedIndex];
        s.setScale(s.scaleX(), (float)sy);
        update();
        emit shapesChanged();
    }
}

void SketchWidget::setSelectedWidth(double w) {
    if (m_selectedIndex >= 0) {
        m_shapes[m_selectedIndex].setWidth((float)w);
        update();
        emit shapesChanged();
    }
}

void SketchWidget::setSelectedHeight(double h) {
    if (m_selectedIndex >= 0) {
        m_shapes[m_selectedIndex].setHeight((float)h);
        update();
        emit shapesChanged();
    }
}

void SketchWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Grid
    painter.setPen(QPen(QColor(60, 60, 80), 1));
    for (int x = 0; x < width(); x += 30)
        painter.drawLine(x, 0, x, height());
    for (int y = 0; y < height(); y += 30)
        painter.drawLine(0, y, width(), y);

    for (const auto& shape : m_shapes)
        drawShape(painter, shape);

    // Preview while drawing
    if (m_drawing) {
        painter.setPen(QPen(Qt::white, 1, Qt::DashLine));
        painter.setBrush(Qt::NoBrush);
        // Already handled via shape list
    }
}

void SketchWidget::drawShape(QPainter& painter, const Shape& shape) {
    QVector<QPointF> verts = shape.getVertices2D();
    QPolygonF poly;
    for (auto& v : verts) poly << v;

    QColor fill = shape.color();
    fill.setAlpha(160);

    QPen pen(shape.isSelected() ? Qt::yellow : shape.color(), shape.isSelected() ? 3 : 2);
    painter.setPen(pen);
    painter.setBrush(QBrush(fill));
    painter.drawPolygon(poly);

    // Label
    painter.setPen(Qt::white);
    painter.setFont(QFont("Courier", 9));
    painter.drawText(shape.origin().toPoint() + QPoint(4, -4), shape.label());
}

void SketchWidget::mousePressEvent(QMouseEvent* event) {
    QPointF pos = event->pos();

    if (m_tool == SketchTool::Select) {
        // Deselect all first
        for (auto& s : m_shapes) s.setSelected(false);
        m_selectedIndex = -1;

        // Find topmost shape under cursor
        for (int i = m_shapes.size() - 1; i >= 0; --i) {
            if (m_shapes[i].contains(pos)) {
                m_shapes[i].setSelected(true);
                m_selectedIndex = i;
                m_dragging  = true;
                m_dragStart = pos;
                m_originStart = m_shapes[i].origin();
                emit shapeSelected(i);
                break;
            }
        }
        update();
        return;
    }

    // Drawing mode
    m_drawing   = true;
    m_drawStart = pos;

    ShapeType type;
    switch (m_tool) {
    case SketchTool::DrawTriangle:  type = ShapeType::Triangle;  break;
    case SketchTool::DrawRectangle: type = ShapeType::Rectangle; break;
    case SketchTool::DrawSquare:    type = ShapeType::Square;    break;
    default: return;
    }

    Shape newShape(type, pos, QColor::fromHsv(QRandomGenerator::global()->bounded(360), 200, 220));
    newShape.setWidth(5);
    newShape.setHeight(5);
    m_shapes.append(newShape);
    m_selectedIndex = m_shapes.size() - 1;
}

void SketchWidget::mouseMoveEvent(QMouseEvent* event) {
    QPointF pos = event->pos();

    if (m_tool == SketchTool::Select && m_dragging && m_selectedIndex >= 0) {
        QPointF delta = pos - m_dragStart;
        m_shapes[m_selectedIndex].setOrigin(m_originStart + delta);
        update();
        emit shapesChanged();
        return;
    }

    if (m_drawing && m_selectedIndex >= 0) {
        float dx = pos.x() - m_drawStart.x();
        float dy = pos.y() - m_drawStart.y();
        float w  = qMax(5.0f, qAbs((float)dx));
        float h  = qMax(5.0f, qAbs((float)dy));
        m_shapes[m_selectedIndex].setWidth(w);
        m_shapes[m_selectedIndex].setHeight(h);
        update();
    }
}

void SketchWidget::mouseReleaseEvent(QMouseEvent*) {
    m_dragging = false;
    m_drawing  = false;
    if (m_selectedIndex >= 0)
        emit shapesChanged();
}
