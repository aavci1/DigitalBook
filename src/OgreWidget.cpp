#include "OgreWidget.h"

#include "OgreManager.h"

#include <QPaintEngine>

#include <OGRE/OgreRenderWindow.h>

class OgreWidgetPrivate {
public:
  OgreWidgetPrivate() : window(0) {
  }

  ~OgreWidgetPrivate() {
  }

  Ogre::RenderWindow *window;
};

OgreWidget::OgreWidget(QWidget *parent) : QWidget(parent), d(new OgreWidgetPrivate()) {
  // enable painting on screen
  setAttribute(Qt::WA_NativeWindow);
  setAttribute(Qt::WA_PaintOnScreen);
  setAttribute(Qt::WA_OpaquePaintEvent);
  // enable mouse tracking
  setMouseTracking(true);
  // set focus policy
  setFocusPolicy(Qt::WheelFocus);
  // accept drops
  setAcceptDrops(true);
}

OgreWidget::~OgreWidget() {
  delete d;
}

Ogre::RenderWindow *OgreWidget::renderWindow() const {
  return d->window;
}

QPaintEngine *OgreWidget::paintEngine() const {
  // Return a null paint engine to disable painting by backing store to pre flicker
  // http://qt.nokia.com/developer/task-tracker/index_html?method=entry&id=128698
  return 0;
}

void OgreWidget::paintEvent(QPaintEvent *) {
  // create the render window if not created yet
  if (!d->window) {
    d->window = OgreManager::instance()->createWindow(this, width(), height());
    // emit signal
    emit windowCreated();
  }
  // update content
  d->window->update();
}

void OgreWidget::resizeEvent(QResizeEvent *) {
  if (!d->window)
    return;
#ifdef Q_WS_X11
  d->window->resize(width(), height());
#endif
  d->window->windowMovedOrResized();
}

void OgreWidget::keyPressEvent(QKeyEvent *e) {
  emit keyPressed(e);
}

void OgreWidget::keyReleaseEvent(QKeyEvent *e) {
  emit keyReleased(e);
}

void OgreWidget::mousePressEvent(QMouseEvent *e) {
  emit mousePressed(e);
}

void OgreWidget::mouseReleaseEvent(QMouseEvent *e) {
  emit mouseReleased(e);
}

void OgreWidget::mouseMoveEvent(QMouseEvent *e) {
  emit mouseMoved(e);
}

void OgreWidget::wheelEvent(QWheelEvent *e) {
  emit wheelMoved(e);
}

void OgreWidget::dragEnterEvent(QDragEnterEvent *e) {
  emit dragEntered(e);
}

void OgreWidget::dragMoveEvent(QDragMoveEvent *e) {
  emit dragMoved(e);
}

void OgreWidget::dragLeaveEvent(QDragLeaveEvent *e) {
  emit dragLeft(e);
}

void OgreWidget::dropEvent(QDropEvent *e) {
  emit dropped(e);
}
