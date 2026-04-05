#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QToolBar>
#include <QDockWidget>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include "sketchwidget.h"
#include "viewer3D.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

private slots:
    void onShapeSelected(int index);
    void onShapesChanged();
    void onColorPickClicked();
    void switchToSketch();
    void switchTo3D();

private:
    void buildToolbar();
    void buildPropertiesPanel();
    void syncPropertiesToShape(int index);

    QTabWidget*     m_tabs;
    SketchWidget*   m_sketch;
    Viewer3D*       m_viewer;

    // Properties Panel
    QDockWidget*    m_propDock;
    QDoubleSpinBox* m_spnScaleX;
    QDoubleSpinBox* m_spnScaleY;
    QDoubleSpinBox* m_spnWidth;
    QDoubleSpinBox* m_spnHeight;
    QPushButton*    m_btnColor;
    QLabel*         m_lblShapeInfo;

    int m_currentShapeIdx = -1;
    bool m_syncingUI = false;
};

#endif // MAINWINDOW_H
