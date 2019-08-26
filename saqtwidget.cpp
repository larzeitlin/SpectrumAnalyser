#include "saqtwidget.h"
#include <QOpenGLShaderProgram>
#include "vertex.h"
#include "cuboidverts.h"

static const  std::array<Vertex, 36> vertexArray = CuboidVerts(1.0f, 1.0f, 1.0f).getVertexArray();

Saqtwidget::Saqtwidget(QWidget *parent) : QOpenGLWidget(parent)
{
    m_transform.translate(0.0f, 0.0f, -5.0f);
}

void Saqtwidget::initializeGL()
{
    // Set up the rendering context
    initializeOpenGLFunctions();

    // this line beneth is to do with triggering redraw, i don't understand it atm.
    connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));

    // Tells openGL to only draw faces that wind anti-clockwise
    glEnable(GL_CULL_FACE);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    m_program = new QOpenGLShaderProgram();
    {
      // Create Shader (Do not release until VAO is created)
      m_program = new QOpenGLShaderProgram();
      m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert");
      m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.frag");
      m_program->link();
      m_program->bind();

      // Cache Uniform Locations (  Waaahh?  )
      u_modelToWorld = m_program->uniformLocation("modelToWorld");
      u_worldToView = m_program->uniformLocation("worldToView");

      // Create Buffer (Do not release until VAO is created)
      m_gl_buffer.create();
      m_gl_buffer.bind();
      m_gl_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);

      m_gl_buffer.allocate(vertexArray.data(), sizeof(vertexArray[0]) * vertexArray.size());

      // m_gl_buffer.allocate(sg_vertices, sizeof(sg_vertices));

      // Create Vertex Array Object
      m_object.create();
      m_object.bind();
      m_program->enableAttributeArray(0);
      m_program->enableAttributeArray(1);
      m_program->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(),
                                    Vertex::PositionTupleSize, Vertex::stride());
      m_program->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(),
                                    Vertex::ColorTupleSize, Vertex::stride());

      // Release (unbind) all
      m_object.release();
      m_gl_buffer.release();
      m_program->release();
    }
}

void Saqtwidget::resizeGL(int w, int h)
{
    m_projection.setToIdentity();
    m_projection.perspective(45.0f, w / float(h), 0.0f, 1000.0f);
}

void Saqtwidget::paintGL()
{
    // Clear
    glClear(GL_COLOR_BUFFER_BIT);

    // Render using our shader
    m_program->bind();
    m_program->setUniformValue(u_worldToView, m_projection);
    {
      m_object.bind();

      // learn more about uniform values in GLSL
      m_program->setUniformValue(u_modelToWorld, m_transform.toMatrix());
      glDrawArrays(GL_TRIANGLES, 0, vertexArray.size());
      m_object.release();
    }
    m_program->release();
}

void Saqtwidget::update()
{
  // Update instance information
  m_transform.rotate(1.0f, QVector3D(0.4f, 0.3f, 0.3f));
//  m_transform.grow(0.001f);

  // Schedule a redraw
  QOpenGLWidget::update();
}

Saqtwidget::~Saqtwidget() {
    m_object.destroy();
    m_gl_buffer.destroy();
    delete m_program;
}
