#include "BookWidget.h"

#include "MainWindow.h"
#include "Page.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QElapsedTimer>
#include <QKeyEvent>
#include <QList>
#include <QTimeLine>

class BookWidgetPrivate {
public:
    BookWidgetPrivate() : timeLine(0), value(0.0f), displayInfo(false), frameCount(0), totalTime(0) {
    }

    ~BookWidgetPrivate() {
        // delete previous pages
        while (!prevPages.isEmpty())
            delete prevPages.takeFirst();
        // delete next pages
        while (!nextPages.isEmpty())
            delete nextPages.takeFirst();
        // delete current page
        delete currentPage;
    }

    QTimeLine *timeLine;
    float value;

    QList<Page *> prevPages;
    QList<Page *> nextPages;
    Page *currentPage;

    bool displayInfo;
    long frameCount;
    long totalTime;
};

BookWidget::BookWidget(QWidget *parent) : QGLWidget(parent), d(new BookWidgetPrivate()) {
    setFocusPolicy(Qt::WheelFocus);
    // create timeline object
    d->timeLine = new QTimeLine(1500, this);
    d->timeLine->setUpdateInterval(15);
    connect(d->timeLine, SIGNAL(valueChanged(qreal)), this, SLOT(animationValueChanged(qreal)));
    connect(d->timeLine, SIGNAL(finished()), this, SLOT(animationFinished()));
}

BookWidget::~BookWidget() {
    delete d;
}

void BookWidget::prevPage() {
    qDebug() << "BookWidget::prevPage();";
    // set current page
    if (!d->currentPage && !d->nextPages.empty()) {
        d->currentPage = d->nextPages.takeFirst();
        d->timeLine->setCurrentTime(0);
    }
    // update timeline direction
    d->timeLine->stop();
    d->timeLine->setDirection(QTimeLine::Forward);
    d->timeLine->resume();
}

void BookWidget::nextPage() {
    qDebug() << "BookWidget::nextPage();";
    // set current page
    if (!d->currentPage && !d->prevPages.empty()) {
        d->currentPage = d->prevPages.takeLast();
        d->timeLine->setCurrentTime(d->timeLine->duration());
    }
    // update timeline direction
    d->timeLine->stop();
    d->timeLine->setDirection(QTimeLine::Backward);
    d->timeLine->resume();
}

void BookWidget::animationValueChanged(qreal value) {
    qDebug() << "BookWidget::animationValueChanged(" << value << ");";
    // update value
    d->value = value;
    // update view
    updateGL();
}

void BookWidget::animationFinished() {
    if (d->currentPage) {
        if (d->timeLine->direction() == QTimeLine::Forward)
            d->prevPages.push_back(d->currentPage);
        else if (d->timeLine->direction() == QTimeLine::Backward)
            d->nextPages.push_front(d->currentPage);
        // reset current page
        d->currentPage = 0;
    }
    // update view
    updateGL();
}

void BookWidget::initializeGL() {
    // initialize glew
    glewInit();
    // set clear color and clear depth
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    // enable depth test
    glEnable(GL_DEPTH_TEST);
    // enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // enable texturing
    glEnable(GL_TEXTURE_2D);
    // file list
    QFileInfoList files = QDir("media").entryInfoList(QDir::Files, QDir::Name | QDir::IgnoreCase);
    // create pages
    for (int i = 0; i < files.size() - 1; i += 2) {
        // read left page data
        QImage left(files.at(i).absoluteFilePath());
        if (left.isNull())
            continue;
        // read right page data
        QImage right(files.at(i + 1).absoluteFilePath());
        if (right.isNull())
            continue;
        // create page
        Page *page = new Page();
        page->setData(Left, left.width(), left.height(), left.bits());
        page->setData(Right, right.width(), right.height(), right.bits());
        // push into the next pages
        d->nextPages.push_back(page);
    }
}

void BookWidget::resizeGL(int width, int height) {
    glViewport(0, 0, width, height);

    // set projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, float(width) / float(height), 1.0f, 500.0f);

    // update view
    updateGL();
}

void BookWidget::paintGL() {
    QElapsedTimer timer;
    timer.start();

    // clear frame buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // select modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (!d->prevPages.empty()) {
        // push matrix
        glPushMatrix();
        // set transformation
        glTranslatef(0.0f, 0.0f, -120.0f);
        glRotatef(-180, 0, 1, 0);
        glTranslatef(26.55f, 0.0f, 0.001f);
        // render page
        d->prevPages.last()->render(Right, 53.1f, 75.0f);
        // pop matrix
        glPopMatrix();
    }
    if (!d->nextPages.empty()) {
        // push matrix
        glPushMatrix();
        // set transformation
        glTranslatef(0.0f, 0.0f, -120.0f);
        glRotatef(0, 0, 1, 0);
        glTranslatef(26.55f, 0.0f, -0.001f);
        // render page
        d->nextPages.first()->render(Left, 53.1f, 75.0f);
        // pop matrix
        glPopMatrix();
    }
    if (d->currentPage) {
        // push matrix
        glPushMatrix();
        // set transformation
        glTranslatef(0.0f, 0.0f, -120.0f);
        glRotatef(-d->value * 180, 0, 1, 0);
        glTranslatef(26.55f, 0.0f, 0.0f);
        // render page
        if (d->value < 0.5f)
            d->currentPage->render(Left, 53.1f, 75.0f);
        else
            d->currentPage->render(Right, 53.1f, 75.0f);
        // pop matrix
        glPopMatrix();
    }

    long elapsed = timer.elapsed();
    // increase frame counter
    d->frameCount++;
    // increase total time
    d->totalTime += elapsed;
    // display current and average frame time
    if (d->displayInfo) {
        renderText(5, height() - 25, QString("Current Frame Time: %1").arg(elapsed, 2), QFont("Monospace", 11));
        renderText(5, height() - 5, QString("Average Frame Time: %1").arg(1.0f * d->totalTime / d->frameCount, 2), QFont("Monospace", 11));
    }
}

void BookWidget::keyPressEvent(QKeyEvent *e) {
    // toggle info display
    if (e->key() == Qt::Key_F1)
        d->displayInfo = !d->displayInfo;
    // page navigation
    if (e->key() == Qt::Key_Left)
        prevPage();
    else if (e->key() == Qt::Key_Right)
        nextPage();
    // toggle video/depth views
    if (e->key() == Qt::Key_F9) {
        MainWindow *mainWindow = dynamic_cast<MainWindow *>(topLevelWidget());
        if (mainWindow) {
            mainWindow->videoWidget->setVisible(!mainWindow->videoWidget->isVisible());
            mainWindow->depthWidget->setVisible(!mainWindow->depthWidget->isVisible());
        }
    }
    // toggle fullscreen
    if ((e->modifiers() == Qt::ControlModifier) && (e->key() == Qt::Key_F)) {
        if (topLevelWidget()->isFullScreen())
            topLevelWidget()->showNormal();
        else
            topLevelWidget()->showFullScreen();
    }
    // update view
    updateGL();
}
