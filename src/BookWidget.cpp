#include "BookWidget.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QKeyEvent>
#include <QList>
#include <QTimeLine>

struct Page {
    float r, g, b;
};

class BookWidgetPrivate {
public:
    BookWidgetPrivate() : timeLine(0), value(0.0f), currentPage(0), displayInfo(0), frameCount(0), totalTime(0) {
        for (uint i = 0; i < 10; ++i) {
            Page * page = new Page();
            page->r = float(qrand()) / RAND_MAX;
            page->g = float(qrand()) / RAND_MAX;
            page->b = float(qrand()) / RAND_MAX;
            nextPages.push_back(page);
        }
    }

    ~BookWidgetPrivate() {
        // release previous pages
        while (!prevPages.empty())
            delete prevPages.takeFirst();
        // release next pages
        while (!nextPages.empty())
            nextPages.takeFirst();
        // release current page
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
    d->timeLine = new QTimeLine(2000, this);
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

void drawQuad(int width, int height) {
    glBegin(GL_QUADS);
    glVertex3f(-0.5f * width, -0.5f * height, 0.0f); glTexCoord2f(1.0f, 0.0f);
    glVertex3f(+0.5f * width, -0.5f * height, 0.0f); glTexCoord2f(1.0f, -1.0f);
    glVertex3f(+0.5f * width, +0.5f * height, 0.0f); glTexCoord2f(0.0f, -1.0f);
    glVertex3f(-0.5f * width, +0.5f * height, 0.0f); glTexCoord2f(0.0f, 0.0f);
    glEnd();
}

void BookWidget::paintGL() {
    QElapsedTimer timer;
    timer.start();

    // clear frame buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // select modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

#if 1
    if (!d->prevPages.empty()) {
        glColor3f(d->prevPages.last()->r,
                  d->prevPages.last()->g,
                  d->prevPages.last()->b);
        glPushMatrix();
        glTranslatef(0.0f, 0.0f, -5.0f);
        glRotatef(-180, 0, 1, 0);
        glTranslatef(1.0f, 0.0f, 0.001f);
        // draw a quad
        drawQuad(2.27f, 3.78f);
        glPopMatrix();
    }
    if (!d->nextPages.empty()) {
        glColor3f(d->nextPages.first()->r,
                  d->nextPages.first()->g,
                  d->nextPages.first()->b);
        glPushMatrix();
        glTranslatef(0.0f, 0.0f, -5.0f);
        glRotatef(0, 0, 1, 0);
        glTranslatef(1.0f, 0.0f, -0.001f);
        // draw a quad
        drawQuad(2.27f, 3.78f);
        glPopMatrix();
    }
    if (d->currentPage) {
        glColor3f(d->currentPage->r,
                  d->currentPage->g,
                  d->currentPage->b);
        glPushMatrix();
        glTranslatef(0.0f, 0.0f, -5.0f);
        glRotatef(-d->value * 180, 0, 1, 0);
        glTranslatef(1.0f, 0.0f, 0.0f);
        // draw a quad
        drawQuad(2.27f, 3.78f);
        glPopMatrix();
    }
#else
    for (uint i = 0; i < d->prevPages.size(); ++i) {
        glColor3f(d->prevPages.at(i)->r,
                  d->prevPages.at(i)->g,
                  d->prevPages.at(i)->b);
        glPushMatrix();
        glTranslatef(-1 - 0.1f * (d->prevPages.size() - i - 1), 0.0f, -5.0f);
        glRotatef(+70, 0, 1, 0);
        // draw a quad
        drawQuad(2.27f, 3.78f);
        glPopMatrix();
    }
    for (uint i = 0; i < d->nextPages.size(); ++i) {
        glColor3f(d->nextPages.at(i)->r,
                  d->nextPages.at(i)->g,
                  d->nextPages.at(i)->b);
        glPushMatrix();
        glTranslatef(+1 + i * 0.1f, 0.0f, -5.0f);
        glRotatef(-70, 0, 1, 0);
        // draw a quad
        drawQuad(2.27f, 3.78f);
        glPopMatrix();
    }
    if (d->currentPage) {
        glColor3f(d->currentPage->r,
                  d->currentPage->g,
                  d->currentPage->b);
        glPushMatrix();
        glTranslatef(-d->value * 2 + 1, 0.0f, -5.0f);
        glRotatef(d->value * 140 - 70, 0, 1, 0);
        // draw a quad
        drawQuad(2.27f, 3.78f);
        glPopMatrix();
    }
#endif

    // increase frame counter
    d->frameCount++;
    // increase total time
    d->totalTime += timer.elapsed();
    // display current and average frame time
    if (d->displayInfo) {
        renderText(10.0f / this->width() - 1.0f, 45.0f / this->height() - 1.0f, 0, QString("Current Frame Time: %1").arg(timer.elapsed(), 2), QFont("Monospace", 12));
        renderText(10.0f / this->width() - 1.0f, 10.0f / this->height() - 1.0f, 0, QString("Average Frame Time: %1").arg(d->totalTime / d->frameCount, 2), QFont("Monospace", 12));
    }
}

void BookWidget::keyPressEvent(QKeyEvent *e) {
    // toggle info display
    if (e->key() == Qt::Key_F1)
        d->displayInfo = !d->displayInfo;
    // update view
    updateGL();
}
