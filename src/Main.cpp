#include <QApplication>

#include "MainWindow.h"

#if defined(Q_WS_WIN32)
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR argv, INT argc) {
  QApplication app(argc, &argv);
#else
int main(int argc, char **argv) {
  QApplication app(argc, argv);
#endif
  // organization and application names, to be used by QSettings
  QCoreApplication::setOrganizationName("BTS3D");
  QCoreApplication::setOrganizationDomain("bts3d.com");
  QCoreApplication::setApplicationName("Digital Library");
  // create main window
  MainWindow main;
  // show main window
  main.showFullScreen();
  // start the event loop
  return app.exec();
}
