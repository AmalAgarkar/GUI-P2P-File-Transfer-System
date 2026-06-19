#include <QApplication>
#include "MainWindow.h"
#include "Style.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("P2P File Transfer");
    app.setOrganizationName("P2P");
    app.setStyleSheet(appStyleSheet());

    MainWindow w;
    w.show();
    return app.exec();
}