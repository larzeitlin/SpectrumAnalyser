#ifndef SAQTWIDGET_H
#define SAQTWIDGET_H

#include<QOpenGLWidget>
#include<QOpenGLFunctions>
#include<QOpenGLBuffer>
#include<QOpenGLVertexArrayObject>


class QOpenGLShaderProgram;


class Saqtwidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    Saqtwidget(QWidget *parent);
    ~Saqtwidget();

    GLuint VertexArrayID;
    GLuint vertexBuffer;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    // OpenGL state info
    QOpenGLBuffer m_vertex;
    QOpenGLVertexArrayObject m_object;
    QOpenGLShaderProgram *m_program;
};

#endif // SAQTWIDGET_H
