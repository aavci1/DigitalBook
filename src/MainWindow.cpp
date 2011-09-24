#include "MainWindow.h"

#include "DepthAnalyzer.h"
#include "KinectManager.h"
#include "OgreManager.h"

#include <OGRE/OgreEntity.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreResourceGroupManager.h>
#include <OGRE/OgreRoot.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>

#include <QDebug>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), mKinectManager(new KinectManager(this)), mDepthAnalyzer(new DepthAnalyzer(this)), mOgreManager(new OgreManager(this)),
mViewport(0), mCamera(0), mBookNode(0), mCurrentSheet(0), mCurrentDirection(DepthAnalyzer::NoDirection), mCurrentAngle(180) {
  setupUi(this);
  // connect signals
  connect(mKinectManager, SIGNAL(videoRetrieved(const quint8*,quint32)), videoWidget, SLOT(updateData(const quint8*,quint32)));
  connect(mKinectManager, SIGNAL(depthRetrieved(const quint16*,quint32)), depthWidget, SLOT(updateData(const quint16*,quint32)));
  connect(mKinectManager, SIGNAL(depthRetrieved(const quint16*,quint32)), mDepthAnalyzer, SLOT(analyze(const quint16*,quint32)));
  connect(mDepthAnalyzer, SIGNAL(swipeRecognized(DepthAnalyzer::Direction)), this, SLOT(swipeRecognized(DepthAnalyzer::Direction)));
  connect(ogreWidget, SIGNAL(windowCreated()), this, SLOT(createScene()));
}

void MainWindow::changeEvent(QEvent *e) {
  QMainWindow::changeEvent(e);
  switch (e->type()) {
  case QEvent::LanguageChange:
    retranslateUi(this);
    break;
  default:
    break;
  }
}

void MainWindow::createScene() {
  // set ambient light
  OgreManager::instance()->sceneManager()->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f));
  // create a camera
  mCamera = OgreManager::instance()->sceneManager()->createCamera("Camera");
  mCamera->setPosition(0.0f, 0.0f, 500.0f);
  mCamera->pitch(Ogre::Degree(0.0f));
  mCamera->setNearClipDistance(0.1f);
  mCamera->setAutoAspectRatio(true);
  // create a viewport
  mViewport = ogreWidget->renderWindow()->addViewport(mCamera);
  mViewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f));
  // create a cube
  mBookNode = OgreManager::instance()->sceneManager()->getRootSceneNode()->createChildSceneNode();
  for (int i = 1; i <= 5; ++i)
    mNextSheets << createSheet(mBookNode, Ogre::Vector3(100.1f, 0.0f, 0.0f), QString("%1.jpg").arg(i));
  for (int i = 1; i < mNextSheets.size(); ++i)
    mNextSheets.at(i)->setVisible(false);
  if (mNextSheets.size() > 0)
    mNextSheets.first()->setVisible(true);
}

Ogre::Entity *MainWindow::createSheet(Ogre::SceneNode *bookNode, Ogre::Vector3 offset, QString texture) {
  // create rotation node
  Ogre::SceneNode *sheetNode = bookNode->createChildSceneNode();
  // create offset node
  Ogre::SceneNode *offsetNode = sheetNode->createChildSceneNode();
  offsetNode->translate(offset);
  offsetNode->scale(2.0f, 3.0f, 0.01f);
  // get the template material
  Ogre::MaterialPtr baseMaterial = Ogre::MaterialManager::getSingleton().createOrRetrieve("Page", "General").first;
  Ogre::MaterialPtr material = baseMaterial->clone((texture + "_material").toStdString());
  material->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName(texture.toStdString());
  material->compile();
  // create sheet entity
  Ogre::Entity *sheet = OgreManager::instance()->sceneManager()->createEntity(Ogre::SceneManager::PT_CUBE);
  sheet->setMaterialName((texture + "_material").toStdString(), "General");
  // attach the sheet to the offset node
  offsetNode->attachObject(sheet);
  // return the sheet object
  return sheet;
}

void MainWindow::swipeRecognized(DepthAnalyzer::Direction direction) {
  qDebug() << "Swipe Recognized" << (direction == DepthAnalyzer::Left ? "Left" : "Right");
  // if current sheet is null, select the appropiate sheet regarding direction
  if (mCurrentSheet == 0) {
    if ((direction == DepthAnalyzer::Left) && (mNextSheets.size() > 0)) {
      mCurrentSheet = mNextSheets.takeFirst();
      mCurrentAngle = 180.0f;
    }
    if ((direction == DepthAnalyzer::Right) && (mPrevSheets.size() > 0)) {
      mCurrentSheet = mPrevSheets.takeLast();
      mCurrentAngle = 0.0f;
    }
  }
  if (mPrevSheets.size() > 0)
    mPrevSheets.last()->setVisible(true);
  if (mNextSheets.size() > 0)
    mNextSheets.first()->setVisible(true);
  // update direction
  mCurrentDirection = direction;
  // start the animation
  turnPage();
}

void MainWindow::turnPage() {
  // if current sheet is null return
  if (mCurrentSheet == 0)
    return;
  // animate using current direction
  if (mCurrentDirection == DepthAnalyzer::Left) {
    // hide the page underneath if the animation is to be completed at this step
    if (mCurrentAngle <= 5.0f && mPrevSheets.size() > 0)
      mPrevSheets.last()->setVisible(false);
    // turn the page a bit
    mCurrentSheet->getParentSceneNode()->getParentSceneNode()->yaw(Ogre::Degree(-5.0f));
    mCurrentAngle -= 5.0f;
    // update view
    ogreWidget->update();
    // add the page into the correct stack
    if (mCurrentAngle <= 0.0f) {
      mPrevSheets.push_back(mCurrentSheet);
      mCurrentSheet = 0;
      return;
    }
  } else if (mCurrentDirection == DepthAnalyzer::Right) {
    // hide the page underneath if the animation is to be completed at this step
    if (mCurrentAngle >= 175.0f && mNextSheets.size() > 0)
      mNextSheets.first()->setVisible(false);
    // turn the page a bit
    mCurrentSheet->getParentSceneNode()->getParentSceneNode()->yaw(Ogre::Degree(+5.0f));
    mCurrentAngle += 5.0f;
    // update view
    ogreWidget->update();
    // add the page into the correct stack
    if (mCurrentAngle >= 180.0f) {
      mNextSheets.push_front(mCurrentSheet);
      mCurrentSheet = 0;
      return;
    }
  }
  // schedule next shot
  QTimer::singleShot(5, this, SLOT(turnPage()));
}
