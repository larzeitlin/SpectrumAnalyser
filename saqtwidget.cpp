#include "saqtwidget.h"
#include <QOpenGLShaderProgram>
#include "vertex.h"



// Front Verticies
#define VERTEX_FTR Vertex( QVector3D( 0.5f,  0.5f,  0.5f), QVector3D( 1.0f, 0.0f, 0.0f ) )
#define VERTEX_FTL Vertex( QVector3D(-0.5f,  0.5f,  0.5f), QVector3D( 0.0f, 1.0f, 0.0f ) )
#define VERTEX_FBL Vertex( QVector3D(-0.5f, -0.5f,  0.5f), QVector3D( 0.0f, 0.0f, 1.0f ) )
#define VERTEX_FBR Vertex( QVector3D( 0.5f, -0.5f,  0.5f), QVector3D( 0.0f, 0.0f, 0.0f ) )

// Back Verticies
#define VERTEX_BTR Vertex( QVector3D( 0.5f,  0.5f, -0.5f), QVector3D( 1.0f, 1.0f, 0.0f ) )
#define VERTEX_BTL Vertex( QVector3D(-0.5f,  0.5f, -0.5f), QVector3D( 0.0f, 1.0f, 1.0f ) )
#define VERTEX_BBL Vertex( QVector3D(-0.5f, -0.5f, -0.5f), QVector3D( 1.0f, 0.0f, 1.0f ) )
#define VERTEX_BBR Vertex( QVector3D( 0.5f, -0.5f, -0.5f), QVector3D( 1.0f, 1.0f, 1.0f ) )

// Create a colored cube
static const Vertex sg_vertexes[] = {
  // Face 1 (Front)
    VERTEX_FTR, VERTEX_FTL, VERTEX_FBL,
    VERTEX_FBL, VERTEX_FBR, VERTEX_FTR,
  // Face 2 (Back)
    VERTEX_BBR, VERTEX_BTL, VERTEX_BTR,
    VERTEX_BTL, VERTEX_BBR, VERTEX_BBL,
  // Face 3 (Top)
    VERTEX_FTR, VERTEX_BTR, VERTEX_BTL,
    VERTEX_BTL, VERTEX_FTL, VERTEX_FTR,
  // Face 4 (Bottom)
    VERTEX_FBR, VERTEX_FBL, VERTEX_BBL,
    VERTEX_BBL, VERTEX_BBR, VERTEX_FBR,
  // Face 5 (Left)
    VERTEX_FBL, VERTEX_FTL, VERTEX_BTL,
    VERTEX_FBL, VERTEX_BTL, VERTEX_BBL,
  // Face 6 (Right)
    VERTEX_FTR, VERTEX_FBR, VERTEX_BBR,
    VERTEX_BBR, VERTEX_BTR, VERTEX_FTR
};

#undef VERTEX_BBR
#undef VERTEX_BBL
#undef VERTEX_BTL
#undef VERTEX_BTR

#undef VERTEX_FBR
#undef VERTEX_FBL
#undef VERTEX_FTL
#undef VERTEX_FTR



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
      m_vertex.create();
      m_vertex.bind();
      m_vertex.setUsagePattern(QOpenGLBuffer::StaticDraw);
      m_vertex.allocate(sg_vertexes, sizeof(sg_vertexes));

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
      m_vertex.release();
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
      m_program->setUniformValue(u_modelToWorld, m_transform.toMatrix());
      glDrawArrays(GL_TRIANGLES, 0, sizeof(sg_vertexes) / sizeof(sg_vertexes[0]));
      m_object.release();
    }
    m_program->release();
}

void Saqtwidget::update()
{
  // Update instance information
  m_transform.rotate(1.0f, QVector3D(0.4f, 0.3f, 0.3f));

  // Schedule a redraw
  QOpenGLWidget::update();
}

Saqtwidget::~Saqtwidget() {
    m_object.destroy();
    m_vertex.destroy();
    delete m_program;
}
