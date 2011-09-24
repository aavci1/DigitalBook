#include "DepthAnalyzer.h"

#include <QPointF>
#include <QSettings>

#include <cmath>

class DepthAnalyzerPrivate {
public:
  DepthAnalyzerPrivate() : lastDirection(DepthAnalyzer::NoDirection), lastCenter(0.0f, 0.0f) {
  }


  DepthAnalyzer::Direction lastDirection;
  QPointF lastCenter;
};

DepthAnalyzer::DepthAnalyzer(QObject *parent) : QObject(parent), d(new DepthAnalyzerPrivate()) {
}

DepthAnalyzer::~DepthAnalyzer() {
  delete d;
}

void DepthAnalyzer::analyze(const quint16 *buffer, const quint32 &timestamp) {
  QSettings settings;
  // far threshold 110 cm converted to kinect depth
  float farThreshold = atan((settings.value("FarThreshold", 110).toInt() - 5.7f) / 33.825) * 1024 - 512;
  // swipe threshold
  float swipeThreshold = settings.value("SwipeThreshold", 50).toInt();
  // calculate the center of mass within the range
  QPointF center;
  int pointCount = 0;
  for (int i = 0; i < 480; ++i) {
    for (int j = 0; j < 640; ++j) {
      // only use points nearer than the far threshold
      if (buffer[i * 640 + j] > farThreshold)
        continue;
      center += QPointF(j, i);
      pointCount += 1;
    }
  }
  // calculate direction
  Direction direction = NoDirection;
  // calculate the center
  if (pointCount) {
    center /= pointCount;
    // check the direction
    if (center.x() > d->lastCenter.x())
      direction = Left;
    else if (center.x() < d->lastCenter.x())
      direction = Right;
  }
  // update direction if last one was none
  if (d->lastDirection == NoDirection) {
    d->lastCenter = center;
    d->lastDirection = direction;
    return;
  }
  if (direction != d->lastDirection) {
    // emit swipe signal if threshold has been reached
    if (std::abs(center.x() - d->lastCenter.x()) > swipeThreshold)
      emit swipeRecognized(d->lastDirection);
    // update start point and direction
    d->lastCenter = center;
    d->lastDirection = direction;
  }
}
