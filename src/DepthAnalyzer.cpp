#include "DepthAnalyzer.h"

#include <QSettings>
#include <QSwipeGesture>

#include <cmath>

class DepthAnalyzerPrivate {
public:
    DepthAnalyzerPrivate() : lastCenterX(0.0f), lastDirection(QSwipeGesture::NoDirection) {
    }

    float lastCenterX;
    QSwipeGesture::SwipeDirection lastDirection;
};

DepthAnalyzer::DepthAnalyzer(QObject *parent) : QObject(parent), d(new DepthAnalyzerPrivate()) {
}

DepthAnalyzer::~DepthAnalyzer() {
    delete d;
}

void DepthAnalyzer::updateData(uchar *image, ushort *depth, int width, int height) {
    QSettings settings;
    // far threshold 200 cm converted to kinect depth
    float farThreshold = atan((settings.value("FarThreshold", 200).toInt() - 5.7f) / 33.825) * 1024 - 512;
    // swipe threshold
    float swipeThreshold = settings.value("SwipeThreshold", 75).toInt();
    // calculate the center of mass within the range
    float centerX = 0, centerY = 0;
    int pointCount = 0;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // only use points nearer than the far threshold
            if ((depth[y * width + x] == 0) || (depth[y * width + x] > farThreshold))
                continue;
            // accumulate coords
            centerX += x;
            centerY += y;
            // increase point count
            pointCount++;
        }
    }
    int minimumPointCount = 400;
    int maxVariation = 50;
    QSwipeGesture::SwipeDirection direction = QSwipeGesture::NoDirection;
    if (pointCount > minimumPointCount) {
        centerX /= pointCount;
        centerY /= pointCount;
        // determine new direction
        if ((centerX - d->lastCenterX) > maxVariation)
            direction = QSwipeGesture::Right;
        else if ((centerX - d->lastCenterX) < -maxVariation)
            direction = QSwipeGesture::Left;
        else
            direction = d->lastDirection;
    }
    // check if direction changed
    if (direction != d->lastDirection) {
        // emit swipe signal if threshold has been reached
        if (qAbs(centerX - d->lastCenterX) > swipeThreshold) {
            if (d->lastDirection == QSwipeGesture::Left)
                emit swipeLeft();
            else if (d->lastDirection == QSwipeGesture::Right)
                emit swipeRight();
        }
        // update start point and direction
        d->lastCenterX = centerX;
        d->lastDirection = direction;
    }
}
