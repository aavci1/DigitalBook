#include "MainWindow.h"

#include <QTimeLine>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);
    // create kinect thread
    kinectThread = new KinectThread(this);
    connect(kinectThread, SIGNAL(captured(uchar*,ushort*,int,int)), this, SLOT(updateData(uchar*,ushort*,int,int)));
    kinectThread->start();
    // create depth analyzer
    depthAnalyzer = new DepthAnalyzer(this);
    connect(depthAnalyzer, SIGNAL(swipeRecognized(DepthAnalyzer::Direction)), this, SLOT(swipeRecognized(DepthAnalyzer::Direction)));
    // create timeline object
    timeLine = new QTimeLine(2000, this);
    timeLine->setUpdateInterval(15);
    connect(timeLine, SIGNAL(valueChanged(qreal)), bookWidget, SLOT(animationValueChanged(qreal)));
    connect(timeLine, SIGNAL(finished()), bookWidget, SLOT(animationFinished()));
}

void MainWindow::updateData(uchar *image, ushort *depth, int width, int height) {
    videoWidget->updateData(image, depth, width, height);
    depthWidget->updateData(image, depth, width, height);
    depthAnalyzer->updateData(image, depth, width, height);
    // clean up
    delete[] image;
    delete[] depth;
}

void MainWindow::swipeRecognized(DepthAnalyzer::Direction direction) {
    timeLine->stop();

    if (direction == DepthAnalyzer::Right)
        timeLine->setDirection(QTimeLine::Backward);
    else if (direction == DepthAnalyzer::Left)
        timeLine->setDirection(QTimeLine::Forward);

    timeLine->resume();
}
