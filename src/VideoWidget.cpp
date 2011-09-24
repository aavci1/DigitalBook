#include "VideoWidget.h"

#include <QPainter>

class VideoWidgetPrivate {
public:
  QImage source;
  QImage image;
  quint32 timestamp;
};

VideoWidget::VideoWidget(QWidget *parent) : QWidget(parent), d(new VideoWidgetPrivate()) {
}

VideoWidget::~VideoWidget() {
  delete d;
}

void VideoWidget::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  // clean background
  painter.fillRect(0, 0, width(), height(), QColor(0, 0, 0));
  // draw image
  painter.drawImage(0, 0, d->image);
}

void VideoWidget::resizeEvent(QResizeEvent *) {
  // update image
  d->image = d->source.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

void VideoWidget::updateData(const quint8 *buffer, const quint32 &timestamp) {
  if (d->timestamp == timestamp)
    return;
  // update image
  d->source = QImage(buffer, 640, 480, QImage::Format_RGB888);
  d->image = d->source.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  // update timestamp
  d->timestamp = timestamp;
  // update view
  update();
}
