#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_MainWindow.h"

#include "DepthAnalyzer.h"
#include "KinectThread.h"

class MainWindow : public QMainWindow, public Ui::MainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

private slots:
    void updateData(uchar *image, ushort *depth, int width, int height);

private:
    KinectThread *kinectThread;
    DepthAnalyzer *depthAnalyzer;
};

#endif // MAINWINDOW_H
