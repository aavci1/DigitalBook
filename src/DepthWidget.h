#ifndef DEPTHWIDGET_H
#define DEPTHWIDGET_H

#include <QWidget>

class DepthWidgetPrivate;

class DepthWidget : public QWidget {
    Q_OBJECT
public:
    DepthWidget(QWidget *parent = 0);
    ~DepthWidget();

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);

public slots:
    void updateData(uchar *image, ushort *depth, int width, int height);

private:
    DepthWidgetPrivate *d;
};

#endif // DEPTHWIDGET_H
