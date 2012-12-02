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
    // near and far thresholds
    float nearThreshold = settings.value("NearThreshold", 500).toInt();
    float farThreshold = settings.value("FarThreshold", 1000).toInt();
    // update image
    d->source = QImage(width, height, QImage::Format_RGB888);
    for (int y = 0; y < height; ++y) {
        quint8 *scanline = d->source.scanLine(y);
        memset(scanline, 0, width * 3);
        for (int x = 0; x < width; ++x) {
            // only use within  the threshold ranges
            if ((depth[y * width + x] == 0) || (depth[y * width + x] < nearThreshold) || (depth[y * width + x] > farThreshold))
                continue;
            // only use points nearer than the far threshold
            quint8 color = 256 * (1.0f - depth[y * width + x] / farThreshold);
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
