#ifndef KINECTTHREAD_H
#define KINECTTHREAD_H

#include <QThread>

class KinectThreadPrivate;

class KinectThread : public QThread {
    Q_OBJECT
public:
    KinectThread(QObject *parent = 0);
    ~KinectThread();

signals:
    void captured(uchar *image, ushort *depth, int width, int height);

protected:
    void run();

private:
    KinectThreadPrivate *d;
};

#endif // KINECTTHREAD_H
