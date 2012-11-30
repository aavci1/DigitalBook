#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>

class VideoWidgetPrivate;

class VideoWidget : public QWidget {
    Q_OBJECT
public:
    VideoWidget(QWidget *parent = 0);
    ~VideoWidget();

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);

public slots:
    void updateData(uchar *image, ushort *depth, int width, int height);

private:
    VideoWidgetPrivate *d;
};

#endif // VIDEOWIDGET_H
