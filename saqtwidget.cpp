#include "saqtwidget.h"
#include <QOpenGLFunctions>

Saqtwidget::Saqtwidget(QWidget *parent) : QOpenGLWidget(parent) {}

void Saqtwidget::initializeGL()
{
    // Set up the rendering context
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void Saqtwidget::resizeGL(int w, int h)
{
    // Update projection matrix and other things related to size.
}


void Saqtwidget::paintGL()
{
    // draw the scene
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT);
}
