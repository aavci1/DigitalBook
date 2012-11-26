#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QThread>

class CameraThreadPrivate;

class CameraThread : public QThread {
    Q_OBJECT
public:
    CameraThread(QObject *parent = 0);
    ~CameraThread();

signals:
    void depthCaptured(const quint16 *data, const quint32 timestamp);
    void videoCaptured(const quint8 *data, const quint32 timestamp);

protected:
    void run();

private:
    CameraThreadPrivate *d;
};

#endif // CAMERATHREAD_H
