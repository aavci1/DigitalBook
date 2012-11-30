#ifndef BOOKWIDGET_H
#define BOOKWIDGET_H

#include <GL/glew.h>
#include <QGLWidget>

class BookWidgetPrivate;

class BookWidget : public QGLWidget {
    Q_OBJECT
public:
    BookWidget(QWidget *parent = 0);
    ~BookWidget();

public slots:
    void animationValueChanged(qreal value);
    void animationFinished();

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

    void keyPressEvent(QKeyEvent *e);

private:
    BookWidgetPrivate *d;
};

#endif // BOOKWIDGET_H
