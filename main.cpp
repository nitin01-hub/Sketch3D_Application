#include <QApplication>
#include "mainwindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Sketch3D");
    app.setOrganizationName("Sketch3D Studio");

    MainWindow window;
    window.show();

    return app.exec();
}
