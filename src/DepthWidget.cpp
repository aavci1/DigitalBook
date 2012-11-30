#include "DepthWidget.h"

#include <QPainter>
#include <QSettings>

#include <cmath>

class DepthWidgetPrivate {
public:
    QImage source;
    QImage image;
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
    if (d->source.isNull())
        return;
    // update image
    d->image = d->source.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

void DepthWidget::updateData(uchar *image, ushort *depth, int width, int height) {
    Q_UNUSED(image);
    // read settings
    QSettings settings;
    // far threshold 200 cm converted to kinect depth
    float farThreshold = atan((settings.value("FarThreshold", 200).toInt() - 5.7f) / 33.825) * 1024 - 512;
    // update image
    d->source = QImage(width, height, QImage::Format_RGB888);
    for (int y = 0; y < height; ++y) {
        quint8 *scanline = d->source.scanLine(y);
        for (int x = 0; x < width; ++x) {
            quint8 color = 0;
            // only use points nearer than the far threshold
            if (depth[y * width + x] != 0 && depth[y * width + x] <= farThreshold)
                color = 256 * (1.0f - depth[y * width + x] / farThreshold);
            // set color
            scanline[x * 3 + 0] = color;
            scanline[x * 3 + 1] = color;
            scanline[x * 3 + 2] = color;
        }
    }
    d->image = d->source.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    // update view
    update();
}
