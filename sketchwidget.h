#ifndef SKETCHWIDGET_H
#define SKETCHWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPointF>
#include "shape.h"

enum class SketchTool {
    Select,
    DrawTriangle,
    DrawRectangle,
    DrawSquare
};

class SketchWidget : public QWidget {
    Q_OBJECT

public:
    explicit SketchWidget(QWidget* parent = nullptr);

    void setTool(SketchTool tool);
    const QVector<Shape>& shapes() const { return m_shapes; }
    void clearAll();

signals:
    void shapeSelected(int index);
    void shapesChanged();

public slots:
    void deleteSelected();
    void setSelectedColor(QColor color);
    void setSelectedScaleX(double sx);
    void setSelectedScaleY(double sy);
    void setSelectedWidth(double w);
    void setSelectedHeight(double h);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QVector<Shape> m_shapes;
    SketchTool     m_tool = SketchTool::Select;
    int            m_selectedIndex = -1;
    bool           m_dragging = false;
    QPointF        m_dragStart;
    QPointF        m_originStart;
    bool           m_drawing = false;
    QPointF        m_drawStart;

    void drawShape(QPainter& painter, const Shape& shape);
};

#endif // SKETCHWIDGET_H
