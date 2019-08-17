#ifndef SAQTWIDGET_H
#define SAQTWIDGET_H

#include<QOpenGLWidget>
#include<QOpenGLFunctions>

class Saqtwidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    Saqtwidget(QWidget *parent);

    GLuint VertexArrayID;
    GLuint vertexBuffer;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
};

#endif // SAQTWIDGET_H
