#include "BookWidget.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QKeyEvent>

class BookWidgetPrivate {
public:
    BookWidgetPrivate() : value(0.0f), displayInfo(0), frameCount(0), totalTime(0) {
    }

    ~BookWidgetPrivate() {
    }

    float value;

    bool displayInfo;
    long frameCount;
    long totalTime;
};

BookWidget::BookWidget(QWidget *parent) : QGLWidget(parent), d(new BookWidgetPrivate()) {
    setFocusPolicy(Qt::WheelFocus);
}

BookWidget::~BookWidget() {
    delete d;
}

void BookWidget::animationValueChanged(qreal value) {
    qDebug() << "BookWidget::animationValueChanged(" << value << ");";
    // update value
    d->value = value;
    // update view
    updateGL();
}

void BookWidget::animationFinished() {
    qDebug() << "BookWidget::animationFinished();";
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
    glTranslatef(0.0f, 0.0f, -5.0f);
    glRotatef(-d->value * 180, 0, 1, 0);
    glTranslatef(1.0f, 0.0f, 0.0f);
#else
    glTranslatef(-d->value * 2 + 1, 0.0f, -5.0f);
    glRotatef(d->value * 140 - 70, 0, 1, 0);
#endif
    // draw a quad
    drawQuad(2.27f, 3.78f);

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
