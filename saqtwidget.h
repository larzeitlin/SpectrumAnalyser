#ifndef SAQTWIDGET_H
#define SAQTWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <transform3d.h>
#include <QTime>
#include <QAudioProbe>
#include "audioprocessor.h"

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
    void processAudioBuffer(QAudioBuffer buffer);


private:
    void ComputePositionOffsets(float &fXOffset, float &fYoffset);

    // OpenGL state info
    QOpenGLBuffer gl_buffer;
    QOpenGLVertexArrayObject vbObject;
    QOpenGLShaderProgram *program;

    // Shader uniforms etc
    int modelToWorld;
    int worldToView;
    int timeElapsed;
    int fftVal;


    QMatrix4x4 projection;
    std::vector<Transform3D> transforms;
    QTime timer;

    AudioProcessor processor;
    std::vector<double> real_spectrum_l;
    std::vector<double> real_spectrum_r;
};

#endif // SAQTWIDGET_H
