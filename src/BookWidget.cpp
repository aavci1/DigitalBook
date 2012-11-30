#include "BookWidget.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QKeyEvent>
#include <QList>
#include <QTimeLine>

class Page {
public:
    Page() : data(0), width(0), height(0) {
    }

    ~Page() {
        delete[] data;
    }

    uchar *data;
    int width;
    int height;
};

class BookWidgetPrivate {
public:
    BookWidgetPrivate() : timeLine(0), value(0.0f), currentPage(0), displayInfo(0), frameCount(0), totalTime(0) {
        for (uint i = 0; i < 5; ++i) {
            QImage image(QString("media/textures/%1.jpg").arg(i + 1));
            if (image.isNull())
                continue;

            Page *page = new Page();
            page->width = image.width();
            page->height = image.height();
            page->data = new uchar[image.byteCount()];
            // copy image data
            memcpy(page->data, image.bits(), image.byteCount());

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
        // delete texture
        glDeleteTextures(1, &texture);
    }

    QTimeLine *timeLine;
    float value;

    QList<Page *> prevPages;
    QList<Page *> nextPages;
    Page *currentPage;

    GLuint texture;

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
    // create textures
    glGenTextures(1, &d->texture);
    // set texture parameters
    glBindTexture(GL_TEXTURE_2D, d->texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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

void renderPage(Page *page, int width, int height) {
    // copy page data to the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, page->width, page->height, 0, GL_BGRA, GL_UNSIGNED_BYTE, page->data);
    // draw quad
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
        // push matrix
        glPushMatrix();
        // set transformation
        glTranslatef(0.0f, 0.0f, -5.0f);
        glRotatef(-180, 0, 1, 0);
        glTranslatef(1.0f, 0.0f, 0.001f);
        // draw a quad
        renderPage(d->prevPages.last(), 2.27f, 3.78f);
        // pop matrix
        glPopMatrix();
    }
    if (!d->nextPages.empty()) {
        // push matrix
        glPushMatrix();
        // set transformation
        glTranslatef(0.0f, 0.0f, -5.0f);
        glRotatef(0, 0, 1, 0);
        glTranslatef(1.0f, 0.0f, -0.001f);
        // draw a quad
        renderPage(d->nextPages.first(), 2.27f, 3.78f);
        // pop matrix
        glPopMatrix();
    }
    if (d->currentPage) {
        // push matrix
        glPushMatrix();
        // set transformation
        glTranslatef(0.0f, 0.0f, -5.0f);
        glRotatef(-d->value * 180, 0, 1, 0);
        glTranslatef(1.0f, 0.0f, 0.0f);
        // draw a quad
        renderPage(d->currentPage, 2.27f, 3.78f);
        // pop matrix
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
    // page navigation
    if (e->key() == Qt::Key_Left)
        prevPage();
    else if (e->key() == Qt::Key_Right)
        nextPage();
    // update view
    updateGL();
}
