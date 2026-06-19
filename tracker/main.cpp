#include <QApplication>
#include "TrackerWindow.h"
#include "Style.h"
 
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("P2P Tracker");
    app.setStyleSheet(appStyleSheet());
 
    TrackerWindow w;
    w.show();
    return app.exec();
}
 