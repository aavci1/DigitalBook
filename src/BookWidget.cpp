#include "BookWidget.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QKeyEvent>
#include <QList>
#include <QTimeLine>

class BookWidgetPrivate {
public:
    BookWidgetPrivate() : timeLine(0), value(0.0f), currentLeft(0), currentRight(0), displayInfo(0), frameCount(0), totalTime(0) {
    }

    ~BookWidgetPrivate() {
        // release previous pages
        while (!prevPages.empty()) {
            GLuint texture = prevPages.takeFirst();
            glDeleteTextures(1, &texture);
        }
        // release next pages
        while (!nextPages.empty()) {
            GLuint texture = nextPages.takeFirst();
            glDeleteTextures(1, &texture);
        }
        // release current page
        glDeleteTextures(1, &currentLeft);
        glDeleteTextures(1, &currentRight);
    }

    QTimeLine *timeLine;
    float value;

    QList<GLuint> prevPages;
    QList<GLuint> nextPages;
    GLuint currentLeft;
    GLuint currentRight;

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
    if (!d->currentLeft && !d->nextPages.empty()) {
        d->currentLeft = d->nextPages.takeFirst();
        d->currentRight = d->nextPages.takeFirst();
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
    if (!d->currentLeft && !d->prevPages.empty()) {
        d->currentRight = d->prevPages.takeLast();
        d->currentLeft = d->prevPages.takeLast();
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
    if (d->currentLeft) {
        if (d->timeLine->direction() == QTimeLine::Forward) {
            d->prevPages.push_back(d->currentLeft);
            d->prevPages.push_back(d->currentRight);
        } else if (d->timeLine->direction() == QTimeLine::Backward) {
            d->nextPages.push_front(d->currentRight);
            d->nextPages.push_front(d->currentLeft);
        }
        // reset current page
        d->currentLeft = 0;
        d->currentRight = 0;
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
    // create textures
    for (int i = 1; i <= 60; ++i) {
        QImage image(QString("media/%1.jpg").arg(QString::number(i), 2, '0'));
        if (image.isNull())
            continue;
        // create texture
        GLuint texture = 0;
        glGenTextures(1, &texture);
        // set texture parameters
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        // set texture data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, image.bits());
        // generate mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);
        // push into the next pages
        d->nextPages.push_back(texture);
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

void renderPage(GLuint texture, float width, float height, bool mirror = false) {
    // copy page data to the texture
    glBindTexture(GL_TEXTURE_2D, texture);
    // draw quad
    glBegin(GL_QUADS);
    glVertex3f(-0.5f * width, -0.5f * height, 0.0f); glTexCoord2f(mirror ? -1.0f : 1.0f, 0.0f);
    glVertex3f(+0.5f * width, -0.5f * height, 0.0f); glTexCoord2f(mirror ? -1.0f : 1.0f, -1.0f);
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

    if (!d->prevPages.empty()) {
        // push matrix
        glPushMatrix();
        // set transformation
        glTranslatef(0.0f, 0.0f, -100.0f);
        glRotatef(-180, 0, 1, 0);
        glTranslatef(26.55f, 0.0f, 0.001f);
        // draw a quad
        renderPage(d->prevPages.last(), 53.1f, 75.0f, true);
        // pop matrix
        glPopMatrix();
    }
    if (!d->nextPages.empty()) {
        // push matrix
        glPushMatrix();
        // set transformation
        glTranslatef(0.0f, 0.0f, -100.0f);
        glRotatef(0, 0, 1, 0);
        glTranslatef(26.55f, 0.0f, -0.001f);
        // draw a quad
        renderPage(d->nextPages.first(), 53.1f, 75.0f);
        // pop matrix
        glPopMatrix();
    }
    if (d->currentLeft && d->currentRight) {
        // push matrix
        glPushMatrix();
        // set transformation
        glTranslatef(0.0f, 0.0f, -100.0f);
        glRotatef(-d->value * 180, 0, 1, 0);
        glTranslatef(26.55f, 0.0f, 0.0f);
        // draw a quad
        if (d->value < 0.5f)
            renderPage(d->currentLeft, 53.1f, 75.0f);
        else
            renderPage(d->currentRight, 53.1f, 75.0f, true);
        // pop matrix
        glPopMatrix();
    }

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
    // page navigation
    if (e->key() == Qt::Key_Left)
        prevPage();
    else if (e->key() == Qt::Key_Right)
        nextPage();
    // update view
    updateGL();
}
