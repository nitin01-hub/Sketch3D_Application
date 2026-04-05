#include "mainwindow.h"
#include <QMenuBar>
#include <QAction>
#include <QColorDialog>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QStatusBar>
#include <QToolButton>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Sketch3D");
    resize(1100, 700);
    setStyleSheet(R"(
        QMainWindow, QWidget {
            background-color: #1e1e2e;
            color: #cdd6f4;
            font-family: 'Consolas', 'Courier New', monospace;
        }
        QToolBar { background: #181825; border: none; spacing: 6px; padding: 4px; }
        QToolButton {
            background: #313244; color: #cdd6f4; border: none;
            border-radius: 6px; padding: 8px 14px; font-size: 13px;
        }
        QToolButton:hover   { background: #45475a; }
        QToolButton:checked { background: #89b4fa; color: #1e1e2e; }
        QDockWidget { background: #181825; border: none; }
        QDockWidget::title { background: #313244; padding: 6px; }
        QDoubleSpinBox, QSpinBox {
            background: #313244; border: 1px solid #45475a;
            border-radius: 4px; padding: 4px; color: #cdd6f4;
        }
        QPushButton {
            background: #313244; color: #cdd6f4; border: none;
            border-radius: 6px; padding: 6px 12px;
        }
        QPushButton:hover { background: #45475a; }
        QTabWidget::pane  { border: 2px solid #313244; }
        QTabBar::tab {
            background: #313244; color: #6c7086; padding: 8px 20px;
            border-top-left-radius: 6px; border-top-right-radius: 6px;
        }
        QTabBar::tab:selected { background: #89b4fa; color: #1e1e2e; }
        QLabel { color: #bac2de; }
        QGroupBox {
            border: 1px solid #45475a; border-radius: 6px;
            margin-top: 8px; padding-top: 8px; color: #89b4fa;
        }
        QGroupBox::title { subcontrol-origin: margin; left: 10px; }
    )");

    // Central Tab Widget
    m_tabs   = new QTabWidget(this);
    m_sketch = new SketchWidget(this);
    m_viewer = new Viewer3D(this);
    m_tabs->addTab(m_sketch, "✏️  Sketch Mode");
    m_tabs->addTab(m_viewer, "🧊  3D View");
    setCentralWidget(m_tabs);

    connect(m_tabs, &QTabWidget::currentChanged, this, [this](int idx) {
        if (idx == 1) onShapesChanged(); // refresh 3D
    });

    connect(m_sketch, &SketchWidget::shapeSelected, this, &MainWindow::onShapeSelected);
    connect(m_sketch, &SketchWidget::shapesChanged,  this, &MainWindow::onShapesChanged);

    buildToolbar();
    buildPropertiesPanel();

    statusBar()->showMessage("Ready  |  Select a tool to start sketching");
    statusBar()->setStyleSheet("background: #181825; color: #6c7086;");
}

void MainWindow::buildToolbar() {
    QToolBar* bar = addToolBar("Tools");
    bar->setMovable(false);

    auto addTool = [&](const QString& label, bool checkable = false) -> QToolButton* {
        auto* btn = new QToolButton();
        btn->setText(label);
        btn->setCheckable(checkable);
        bar->addWidget(btn);
        return btn;
    };

    auto* btnSelect = addTool("⬆ Select", true);
    auto* btnTri    = addTool("▲ Triangle", true);
    auto* btnRect   = addTool("▬ Rectangle", true);
    auto* btnSq     = addTool("■ Square", true);

    bar->addSeparator();
    auto* btnDel    = addTool("🗑 Delete");
    auto* btnClear  = addTool("✖ Clear All");
    bar->addSeparator();
    auto* btn2D     = addTool("✏️ Sketch", true);
    auto* btn3D     = addTool("🧊 3D View", true);

    // Tool group
    QList<QToolButton*> drawTools = { btnSelect, btnTri, btnRect, btnSq };
    auto setActiveTool = [=](QToolButton* active, SketchTool tool) {
        for (auto* b : drawTools) b->setChecked(false);
        active->setChecked(true);
        m_sketch->setTool(tool);
    };

    connect(btnSelect, &QToolButton::clicked, this, [=]() { setActiveTool(btnSelect, SketchTool::Select); });
    connect(btnTri,    &QToolButton::clicked, this, [=]() { setActiveTool(btnTri,    SketchTool::DrawTriangle); });
    connect(btnRect,   &QToolButton::clicked, this, [=]() { setActiveTool(btnRect,   SketchTool::DrawRectangle); });
    connect(btnSq,     &QToolButton::clicked, this, [=]() { setActiveTool(btnSq,     SketchTool::DrawSquare); });

    connect(btnDel,   &QToolButton::clicked, m_sketch, &SketchWidget::deleteSelected);
    connect(btnClear, &QToolButton::clicked, m_sketch, &SketchWidget::clearAll);
    connect(btn2D, &QToolButton::clicked, this, [=]() {
        btn2D->setChecked(true); btn3D->setChecked(false);
        m_tabs->setCurrentIndex(0);
    });
    connect(btn3D, &QToolButton::clicked, this, [=]() {
        btn3D->setChecked(true); btn2D->setChecked(false);
        m_tabs->setCurrentIndex(1);
        onShapesChanged();
    });

    btnSelect->setChecked(true);
}

void MainWindow::buildPropertiesPanel() {
    m_propDock = new QDockWidget("Properties", this);
    m_propDock->setAllowedAreas(Qt::RightDockWidgetArea);
    m_propDock->setFeatures(QDockWidget::NoDockWidgetFeatures);

    auto* container = new QWidget();
    auto* layout    = new QVBoxLayout(container);
    layout->setSpacing(12);

    m_lblShapeInfo = new QLabel("No shape selected");
    m_lblShapeInfo->setStyleSheet("color: #89b4fa; font-weight: bold; font-size: 13px;");
    layout->addWidget(m_lblShapeInfo);

    // Geometry Group
    auto* geoGroup  = new QGroupBox("Geometry");
    auto* geoLayout = new QFormLayout(geoGroup);

    m_spnWidth  = new QDoubleSpinBox(); m_spnWidth->setRange(10, 1000);  m_spnWidth->setValue(100);
    m_spnHeight = new QDoubleSpinBox(); m_spnHeight->setRange(10, 1000); m_spnHeight->setValue(100);
    geoLayout->addRow("Width:",  m_spnWidth);
    geoLayout->addRow("Height:", m_spnHeight);
    layout->addWidget(geoGroup);

    // Scale Group
    auto* scaleGroup  = new QGroupBox("Scale");
    auto* scaleLayout = new QFormLayout(scaleGroup);
    m_spnScaleX = new QDoubleSpinBox(); m_spnScaleX->setRange(0.1, 10.0); m_spnScaleX->setSingleStep(0.1); m_spnScaleX->setValue(1.0);
    m_spnScaleY = new QDoubleSpinBox(); m_spnScaleY->setRange(0.1, 10.0); m_spnScaleY->setSingleStep(0.1); m_spnScaleY->setValue(1.0);
    scaleLayout->addRow("Scale X:", m_spnScaleX);
    scaleLayout->addRow("Scale Y:", m_spnScaleY);
    layout->addWidget(scaleGroup);

    // Color
    auto* colorGroup  = new QGroupBox("Appearance");
    auto* colorLayout = new QVBoxLayout(colorGroup);
    m_btnColor = new QPushButton("Pick Color");
    m_btnColor->setStyleSheet("background: #89b4fa; color: #1e1e2e; font-weight: bold; border-radius: 6px; padding: 8px;");
    colorLayout->addWidget(m_btnColor);
    layout->addWidget(colorGroup);

    layout->addStretch();

    // Connect signals
    connect(m_spnWidth,  QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double v) {
        if (!m_syncingUI) m_sketch->setSelectedWidth(v);
    });
    connect(m_spnHeight, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double v) {
        if (!m_syncingUI) m_sketch->setSelectedHeight(v);
    });
    connect(m_spnScaleX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double v) {
        if (!m_syncingUI) m_sketch->setSelectedScaleX(v);
    });
    connect(m_spnScaleY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double v) {
        if (!m_syncingUI) m_sketch->setSelectedScaleY(v);
    });
    connect(m_btnColor, &QPushButton::clicked, this, &MainWindow::onColorPickClicked);

    m_propDock->setWidget(container);
    addDockWidget(Qt::RightDockWidgetArea, m_propDock);
}

void MainWindow::onShapeSelected(int index) {
    m_currentShapeIdx = index;
    syncPropertiesToShape(index);
}

void MainWindow::onShapesChanged() {
    m_viewer->setShapes(m_sketch->shapes());
    int count = m_sketch->shapes().size();
    statusBar()->showMessage(QString("Shapes: %1").arg(count));
}

void MainWindow::onColorPickClicked() {
    QColor color = QColorDialog::getColor(Qt::blue, this, "Pick Shape Color");
    if (color.isValid()) {
        m_sketch->setSelectedColor(color);
        m_btnColor->setStyleSheet(QString("background: %1; color: white; font-weight: bold;"
                                          "border-radius: 6px; padding: 8px;").arg(color.name()));
    }
}

void MainWindow::syncPropertiesToShape(int index) {
    const QVector<Shape>& shapes = m_sketch->shapes();
    if (index < 0 || index >= shapes.size()) {
        m_lblShapeInfo->setText("No shape selected");
        return;
    }
    m_syncingUI = true;
    const Shape& s = shapes[index];
    m_lblShapeInfo->setText(QString("Selected: %1").arg(s.label()));
    m_spnWidth->setValue(s.width());
    m_spnHeight->setValue(s.height());
    m_spnScaleX->setValue(s.scaleX());
    m_spnScaleY->setValue(s.scaleY());
    m_btnColor->setStyleSheet(QString("background: %1; color: white; font-weight: bold;"
                                      "border-radius: 6px; padding: 8px;").arg(s.color().name()));
    m_syncingUI = false;
}

void MainWindow::switchToSketch() { m_tabs->setCurrentIndex(0); }
void MainWindow::switchTo3D()     { m_tabs->setCurrentIndex(1); onShapesChanged(); }
