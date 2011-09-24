#ifndef OGREWIDGET_H
#define OGREWIDGET_H

#include <QWidget>

#include <OGRE/OgrePrerequisites.h>

class QPaintEngine;

class OgreWidgetPrivate;

class OgreWidget : public QWidget {
  Q_OBJECT
public:
  OgreWidget(QWidget *parent = 0);
  ~OgreWidget();

public:
  Ogre::RenderWindow *renderWindow() const;

protected:
  QPaintEngine *paintEngine() const;

  void paintEvent(QPaintEvent *);
  void resizeEvent(QResizeEvent *);

  void keyPressEvent(QKeyEvent *e);
  void keyReleaseEvent(QKeyEvent *e);

  void mouseMoveEvent(QMouseEvent *e);
  void mousePressEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);

  void wheelEvent(QWheelEvent *e);

  void dragEnterEvent(QDragEnterEvent *e);
  void dragMoveEvent(QDragMoveEvent *e);
  void dragLeaveEvent(QDragLeaveEvent *e);
  void dropEvent(QDropEvent *e);

signals:
  void windowCreated();

  void keyPressed(QKeyEvent *e);
  void keyReleased(QKeyEvent *e);

  void mouseMoved(QMouseEvent *e);
  void mousePressed(QMouseEvent *e);
  void mouseReleased(QMouseEvent *e);

  void wheelMoved(QWheelEvent *e);

  void dragEntered(QDragEnterEvent *e);
  void dragMoved(QDragMoveEvent *e);
  void dragLeft(QDragLeaveEvent *e);
  void dropped(QDropEvent *e);

private:
  OgreWidgetPrivate *d;
};

#endif // OGREWIDGET_H
