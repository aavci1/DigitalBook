#include "CameraThread.h"

#include <libfreenect/libfreenect.h>
#include <libfreenect/libfreenect_sync.h>

class CameraThreadPrivate {
public:
    CameraThreadPrivate() : abort(false) {
    }

    ~CameraThreadPrivate() {
    }

    bool abort;
};

CameraThread::CameraThread(QObject *parent) : QThread(parent), d(new CameraThreadPrivate()) {
}

CameraThread::~CameraThread() {
    d->abort = true;

    wait();

    delete d;
}

void CameraThread::run() {
    freenect_context *context = 0;
    // initialize freenect
    if (freenect_init(&context, 0) < 0)
        return;
    // turn on the red led
    freenect_sync_set_led(LED_RED, 0);
    // loop until aborted
    while (!d->abort) {
        // timestamp
        quint32 timestamp;
        // depth buffer
        quint16 *depth;
        // retrieve depth
        if (freenect_sync_get_depth((void**)&depth, &timestamp, 0, FREENECT_DEPTH_11BIT) == 0)
          emit depthCaptured(depth, timestamp);
        // video buffer
        quint8 *video;
        // retrieve video
        if (freenect_sync_get_video((void**)&video, &timestamp, 0, FREENECT_VIDEO_RGB) == 0)
          emit videoCaptured(video, timestamp);
    }
    // turn off the leds
    freenect_sync_set_led(LED_BLINK_GREEN, 0);
    // stop
    freenect_sync_stop();
    // shut down freenect
    freenect_shutdown(context);
}
