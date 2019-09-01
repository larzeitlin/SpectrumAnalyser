#include "saqtwidget.h"
#include <QOpenGLShaderProgram>
#include "vertex.h"
#include "cuboidverts.h"
#include <math.h>
#include "transform3d.h"

static const  std::array<Vertex, 36> vertexArray = CuboidVerts(1.0f, 1.0f, 1.0f).getVertexArray();


Saqtwidget::Saqtwidget(QWidget *parent) : QOpenGLWidget(parent)
{
    int nboxes = 20;
    timer.start();
    for (int i = 0; i < nboxes; ++i) {
        transforms.push_back(Transform3D());
        transforms.back().translate(0.0f, 0.05f * i, -5.0f);
        transforms.back().rotate(5.0f * i, -1.0f, -1.0f, 1.0f);
    }
}

void Saqtwidget::initializeGL()
{
    // Set up the rendering context
    initializeOpenGLFunctions();

    connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));

    // Tells openGL to only draw faces that wind anti-clockwise
    glEnable(GL_CULL_FACE);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    program = new QOpenGLShaderProgram();
    {
      // Create Shader (Do not release until VAO is created)
      program = new QOpenGLShaderProgram();
      program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert");
      program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.frag");
      program->link();
      program->bind();

      // Cache Uniform Locations (  Waaahh?  )
      modelToWorld = program->uniformLocation("modelToWorld");
      worldToView = program->uniformLocation("worldToView");
      timeElapsed = program->uniformLocation("timeElapsedInMillis");

      // Create Buffer (Do not release until VAO is created)
      gl_buffer.create();
      gl_buffer.bind();
      gl_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
      // consider StreamDraw when changing buffer data... https://paroj.github.io/gltut/Positioning/Tutorial%2003.html

      gl_buffer.allocate(vertexArray.data(), sizeof(vertexArray[0]) * vertexArray.size());


      // Create Vertex Array Object
      vbObject.create();
      vbObject.bind();
      program->enableAttributeArray(0);
      program->enableAttributeArray(1);
      program->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(),
                                    Vertex::PositionTupleSize, Vertex::stride());
      program->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(),
                                    Vertex::ColorTupleSize, Vertex::stride());

      // Release (unbind) all
      vbObject.release();
      gl_buffer.release();
      program->release();
    }
}

void Saqtwidget::resizeGL(int w, int h)
{
    projection.setToIdentity();
    projection.perspective(45.0f, w / float(h), 0.0f, 1000.0f);
}

void Saqtwidget::paintGL()
{
    float timeInMillis = timer.elapsed();
    // Clear
    glClear(GL_COLOR_BUFFER_BIT);
    // Render using our shader
    program->bind();
    program->setUniformValue(worldToView, projection);
    glUniform1f(timeElapsed, timeInMillis);
    vbObject.bind();
    for (auto t : transforms) {
        program->setUniformValue(modelToWorld, t.toMatrix());
        glDrawArrays(GL_TRIANGLES, 0, vertexArray.size());
    }



    vbObject.release();
    program->release();
}

void Saqtwidget::update()
{
  // Schedule a redraw
  QOpenGLWidget::update();
}

Saqtwidget::~Saqtwidget() {
    vbObject.destroy();
    gl_buffer.destroy();
    delete program;
}

void Saqtwidget::processAudioBuffer(QAudioBuffer buffer)
{
    const quint16 *data = buffer.constData<quint16>();
    qInfo() << data[0] << "\n";
}
