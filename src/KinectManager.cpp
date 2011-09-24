#include "KinectManager.h"

#include <QTimer>

#include <libfreenect/libfreenect.h>
#include <libfreenect/libfreenect_sync.h>

class KinectManagerPrivate {
public:
  KinectManagerPrivate() : context(0) {
  }

  freenect_context *context;
};

KinectManager::KinectManager(QObject *parent) : QObject(parent), d(new KinectManagerPrivate()) {
  if (freenect_init(&d->context, NULL) < 0)
    return;
  // turn on the red led
  freenect_sync_set_led(LED_RED, 0);
  // shoot timer
  QTimer::singleShot(10, this, SLOT(retrieve()));
}

KinectManager::~KinectManager() {
  // turn off the leds
  freenect_sync_set_led(LED_BLINK_GREEN, 0);
  if (d->context) {
    freenect_sync_stop();
    freenect_shutdown(d->context);
  }
  delete d;
}

void KinectManager::retrieve() {
  // timestamp
  quint32 timestamp;
  // depth buffer
  quint16 *depth;
  // retrieve depth
  if (freenect_sync_get_depth((void**)&depth, &timestamp, 0, FREENECT_DEPTH_11BIT) == 0)
    emit depthRetrieved(depth, timestamp);
  // video buffer
  quint8 *video;
  // retrieve video
  if (freenect_sync_get_video((void**)&video, &timestamp, 0, FREENECT_VIDEO_RGB) == 0)
    emit videoRetrieved(video, timestamp);
  // shoot timer
  QTimer::singleShot(10, this, SLOT(retrieve()));
}
