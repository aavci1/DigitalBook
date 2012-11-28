#include "KinectThread.h"

#include <XnCppWrapper.h>

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
    // initialize kinect
    xn::Context context;
    xn::ScriptNode scriptNode;
    xn::EnumerationErrors errors;
    XnStatus status = context.InitFromXmlFile("media/XnConfig.xml", scriptNode, &errors);
    // check status
    if (status == XN_STATUS_NO_NODE_PRESENT) {
        XnChar strError[1024];
        errors.ToString(strError, 1024);
        printf("%s\n", strError);
        return;
    } else if (status != XN_STATUS_OK) {
        printf("Open failed: %s\n", xnGetStatusString(status));
        return;
    }

    // find depth generator node
    xn::DepthGenerator depthGenerator;
    status = context.FindExistingNode(XN_NODE_TYPE_DEPTH, depthGenerator);
    if (status != XN_STATUS_OK) {
        printf("No depth node exists! Check your XML.");
        return;
    }
    // set depth metadata
    xn::DepthMetaData depthMetaData;
    depthGenerator.GetMetaData(depthMetaData);

    // find image node
    xn::ImageGenerator imageGenerator;
    status = context.FindExistingNode(XN_NODE_TYPE_IMAGE, imageGenerator);
    if (status != XN_STATUS_OK) {
        printf("No image node exists! Check your XML.");
        return;
    }
    // set image metadata
    xn::ImageMetaData imageMetaData;
    imageGenerator.GetMetaData(imageMetaData);

    // make sure that resolutions are equal
    if (imageMetaData.FullXRes() != depthMetaData.FullXRes() || imageMetaData.FullYRes() != depthMetaData.FullYRes()) {
        printf("The device depth and image resolution must be equal!\n");
        return;
    }
    int width = imageMetaData.FullXRes();
    int height = imageMetaData.FullYRes();

    // check image pixel format
    if (imageMetaData.PixelFormat() != XN_PIXEL_FORMAT_RGB24) {
        printf("The device image format must be RGB24\n");
        return;
    }

    // loop until aborted
    while (!d->abort) {
        // get a new frame
        XnStatus rc = context.WaitAnyUpdateAll();
        if (rc != XN_STATUS_OK) {
            printf("Read failed: %s\n", xnGetStatusString(rc));
            break;
        }
        // get image data
        imageGenerator.GetMetaData(imageMetaData);
        depthGenerator.GetMetaData(depthMetaData);
        // create buffers
        uchar *imageData = new uchar[width * height * 3];
        ushort *depthData = new ushort[width * height];
        // copy data
        memcpy(imageData, imageMetaData.RGB24Data(), width * height * 3);
        memcpy(depthData, depthMetaData.Data(), width * height * sizeof(ushort));
        // emit signal
        emit captured(imageData, depthData, width, height);
    }
}
