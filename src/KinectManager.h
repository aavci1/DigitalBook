#ifndef KINECTMANAGER_H
#define KINECTMANAGER_H

#include <QObject>

class KinectManagerPrivate;

class KinectManager : public QObject {
  Q_OBJECT

public:
  KinectManager(QObject *parent = 0);
  ~KinectManager();

  static KinectManager *instance();

private slots:
  void retrieve();

signals:
  void depthRetrieved(const quint16 *buffer, const quint32 &timestamp);
  void videoRetrieved(const quint8 *buffer, const quint32 &timestamp);

private:
  KinectManagerPrivate *d;
};

#endif // KINECTMANAGER_H
