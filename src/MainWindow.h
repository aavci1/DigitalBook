#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_MainWindow.h"

#include "DepthAnalyzer.h"
#include "KinectThread.h"

class QTimeLine;

class MainWindow : public QMainWindow, private Ui::MainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = 0);

private slots:
  void updateData(uchar *image, ushort *depth, int width, int height);
  void swipeRecognized(DepthAnalyzer::Direction direction);

private:
  KinectThread *kinectThread;
  DepthAnalyzer *depthAnalyzer;
  QTimeLine *timeLine;
};

#endif // MAINWINDOW_H
