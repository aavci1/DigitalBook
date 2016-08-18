#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);
    // hide video/depth widgets by default
    videoWidget->setVisible(false);
    depthWidget->setVisible(false);
    // create kinect thread
    kinectThread = new KinectThread(this);
    connect(kinectThread, SIGNAL(captured(uchar*,ushort*,int,int)), this, SLOT(updateData(uchar*,ushort*,int,int)));
    kinectThread->start();
    // create depth analyzer
    depthAnalyzer = new DepthAnalyzer(this);
    connect(depthAnalyzer, SIGNAL(swipeLeft()), bookWidget, SLOT(prevPage()));
    connect(depthAnalyzer, SIGNAL(swipeRight()), bookWidget, SLOT(nextPage()));
}

void MainWindow::updateData(uchar *image, ushort *depth, int width, int height) {
    videoWidget->updateData(image, depth, width, height);
    depthWidget->updateData(image, depth, width, height);
    depthAnalyzer->updateData(image, depth, width, height);
    // clean up
    delete[] image;
    delete[] depth;
}
