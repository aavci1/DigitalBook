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
  void updateData(const quint16 *buffer, const quint32 timestamp);

private:
  DepthWidgetPrivate *d;
};

#endif // DEPTHWIDGET_H
