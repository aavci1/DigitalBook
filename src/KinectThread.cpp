#include "KinectThread.h"

#include <OpenNI.h>

using namespace openni;

class KinectThreadPrivate {
public:
    KinectThreadPrivate() : abort(false) {
    }

    ~KinectThreadPrivate() {
    }

    bool abort;
};

KinectThread::KinectThread(QObject *parent) : QThread(parent), d(new KinectThreadPrivate()) {
}

KinectThread::~KinectThread() {
    d->abort = true;

    wait();

    delete d;
}

void KinectThread::run() {
    // initialize OpenNI
    OpenNI::initialize();

    // open device
    Device device;
    device.open(ANY_DEVICE);

    // create depth stream
    VideoStream depthStream;
    depthStream.create(device, SENSOR_IR);

    // start depth stream
    depthStream.start();

    // depth frame
    VideoFrameRef depthFrame;

    while (!d->abort) {
        if (device.getSensorInfo(SENSOR_IR) != NULL) {
            // read frame
            depthStream.readFrame(&depthFrame);

            if (depthFrame.isValid()) {
                // get depth data
                const uint16_t *depthData = (const uint16_t *)depthFrame.getData();

                // get frame width and height
                int width = depthFrame.getWidth(), height = depthFrame.getHeight();

                // create depth buffer
                uint16_t *depthBuffer = new uint16_t[width * height];

                // copy data to the buffer
                memcpy(depthBuffer, depthData, width * height * sizeof(uint16_t));
            }
        }

//        // create buffers
//        uchar *imageData = new uchar[width * height * 3];
//        ushort *depthData = new ushort[width * height];
//        // copy data
//        memcpy(imageData, imageMetaData.RGB24Data(), width * height * 3);
//        memcpy(depthData, depthMetaData.Data(), width * height * sizeof(ushort));
//        // emit signal
//        emit captured(imageData, depthData, width, height);
    }

    // stop depth stream
    depthStream.stop();
    depthStream.destroy();

    // close device
    device.close();
}
