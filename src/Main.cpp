#include "MainWindow.h"

#include <QApplication>
#include <QLocale>

#if defined(Q_WS_WIN32)

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR argv, INT argc) {
    QApplication app(argc, &argv);
#else
int main(int argc, char **argv) {
    QApplication app(argc, argv);
#endif
    // set company and product info
    QCoreApplication::setOrganizationName("BTS3D");
    QCoreApplication::setOrganizationDomain("www.bts3d.com");
    QCoreApplication::setApplicationName("Digital Book");
    QCoreApplication::setApplicationVersion("2012");
    // set default locale to english
    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
    // create main window
    MainWindow main;
    // show the widget
    main.showFullScreen();
    // start appliation event loop
    return app.exec();
}
