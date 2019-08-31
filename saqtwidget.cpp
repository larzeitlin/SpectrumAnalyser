#include "saqtwidget.h"
#include <QOpenGLShaderProgram>
#include "vertex.h"
#include "cuboidverts.h"
#include <math.h>

static const  std::array<Vertex, 36> vertexArray = CuboidVerts(1.0f, 1.0f, 1.0f).getVertexArray();


Saqtwidget::Saqtwidget(QWidget *parent) : QOpenGLWidget(parent)
{
    timer.start();
    transform.translate(0.0f, 0.0f, -5.0f);
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

      // m_gl_buffer.allocate(sg_vertices, sizeof(sg_vertices));

      // Create Vertex Array Object
      object.create();
      object.bind();
      program->enableAttributeArray(0);
      program->enableAttributeArray(1);
      program->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(),
                                    Vertex::PositionTupleSize, Vertex::stride());
      program->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(),
                                    Vertex::ColorTupleSize, Vertex::stride());

      // Release (unbind) all
      object.release();
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
    object.bind();

      program->setUniformValue(modelToWorld, transform.toMatrix());
      glDrawArrays(GL_TRIANGLES, 0, vertexArray.size());

    object.release();
    program->release();
}

void Saqtwidget::update()
{
  // Schedule a redraw
  QOpenGLWidget::update();
}

Saqtwidget::~Saqtwidget() {
    object.destroy();
    gl_buffer.destroy();
    delete program;
}
