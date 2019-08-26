#ifndef SAQTWIDGET_H
#define SAQTWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <transform3d.h>

class QOpenGLShaderProgram;

class Saqtwidget : public QOpenGLWidget,
                   protected QOpenGLFunctions

{
    Q_OBJECT

public:
    Saqtwidget(QWidget *parent);
    ~Saqtwidget() override;

    GLuint VertexArrayID;
    GLuint vertexBuffer;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void teardownGL();

protected slots:
    void update();


private:
    // OpenGL state info
    QOpenGLBuffer m_gl_buffer;
    QOpenGLVertexArrayObject m_object;
    QOpenGLShaderProgram *m_program;

    // Shader info
    int u_modelToWorld;
    int u_worldToView;
    QMatrix4x4 m_projection;
    Transform3D m_transform;

};

#endif // SAQTWIDGET_H
