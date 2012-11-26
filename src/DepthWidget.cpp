#include "DepthWidget.h"

#include <QPainter>
#include <QSettings>

#include <cmath>

class DepthWidgetPrivate {
public:
  QImage source;
  QImage image;
  quint32 timestamp;
};

DepthWidget::DepthWidget(QWidget *parent) : QWidget(parent), d(new DepthWidgetPrivate()) {
}

DepthWidget::~DepthWidget() {
  delete d;
}

void DepthWidget::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  // clean background
  painter.fillRect(0, 0, width(), height(), QColor(0, 0, 0));
  // draw image
  painter.drawImage(0, 0, d->image);
}

void DepthWidget::resizeEvent(QResizeEvent *) {
  // update image
  d->image = d->source.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

void DepthWidget::updateData(const quint16 *buffer, const quint32 &timestamp) {
  if (d->timestamp == timestamp)
    return;
  // read settings
  QSettings settings;
  // far threshold 110 cm converted to kinect depth
  float farThreshold = atan((settings.value("FarThreshold", 110).toInt() - 5.7f) / 33.825) * 1024 - 512;
  // update image
  d->source = QImage(640, 480, QImage::Format_RGB888);
  for (int i = 0; i < 480; ++i) {
    quint8 *scanline = d->source.scanLine(i);
    for (int j = 0; j < 640; ++j) {
      quint8 color = 0;
      if (buffer[i * 640 + j] < farThreshold)
        color = (255 - ((buffer[i * 640 + j] >> 1) & 0x00ff));
      scanline[j * 3 + 0] = color;
      scanline[j * 3 + 1] = color;
      scanline[j * 3 + 2] = color;
    }
  }
  d->image = d->source.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  // update timestamp
  d->timestamp = timestamp;
  // update view
  update();
}
