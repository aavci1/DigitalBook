#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_MainWindow.h"

#include <DepthAnalyzer.h>

#include <OGRE/OgrePrerequisites.h>

class MainWindow : public QMainWindow, private Ui::MainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = 0);

protected:
  void changeEvent(QEvent *e);

private:
  Ogre::Entity *createSheet(Ogre::SceneNode *bookNode, Ogre::Vector3 offset, QString texture);

private slots:
  void swipeRecognized(DepthAnalyzer::Direction direction);
  void createScene();
  void turnPage();

private:
  /// sheets on the left side
  QList<Ogre::Entity *> mPrevSheets;
  /// sheets on the right side
  QList<Ogre::Entity *> mNextSheets;
  /// currently animating sheet
  Ogre::Entity *mCurrentSheet;
  DepthAnalyzer::Direction mCurrentDirection;
  int mCurrentAngle;
};

#endif // MAINWINDOW_H
