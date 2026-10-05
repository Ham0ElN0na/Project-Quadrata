#include <QApplication>
#include "MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Quadrata");

    // Base font
    QFont f = app.font();
    f.setFamily("Segoe UI");
    f.setPointSize(10);
    app.setFont(f);

    MainWindow w;
    w.show();
    return app.exec();
}
