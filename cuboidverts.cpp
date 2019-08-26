#include "cuboidverts.h"
#include <QVector3D>

const QVector3D CuboidVerts::red   { QVector3D( 1.0f, 0.0f, 0.0f ) };
const QVector3D CuboidVerts::green { QVector3D( 0.0f, 1.0f, 0.0f ) };
const QVector3D CuboidVerts::blue  { QVector3D( 0.0f, 0.0f, 1.0f ) };
const QVector3D CuboidVerts::black { QVector3D( 0.0f, 0.0f, 0.0f ) };
const QVector3D CuboidVerts::white { QVector3D( 1.0f, 1.0f, 1.0f ) };

CuboidVerts::CuboidVerts(float h, float w, float d)
  : m_half_height(h / 2.0f)
  , m_half_width(w / 2.0f)
  , m_half_depth(d / 2.0f)

{
    // Front Verticies
    Vertex ftr = Vertex( QVector3D( m_half_height,  m_half_width,  m_half_depth), CuboidVerts::red   );
    Vertex ftl = Vertex( QVector3D(-m_half_height,  m_half_width,  m_half_depth), CuboidVerts::white );
    Vertex fbl = Vertex( QVector3D(-m_half_height, -m_half_width,  m_half_depth), CuboidVerts::blue  );
    Vertex fbr = Vertex( QVector3D( m_half_height, -m_half_width,  m_half_depth), CuboidVerts::black );
    // Back Verticies
    Vertex btr = Vertex( QVector3D( m_half_height,  m_half_width, -m_half_depth), CuboidVerts::blue  );
    Vertex btl = Vertex( QVector3D(-m_half_height,  m_half_width, -m_half_depth), CuboidVerts::black );
    Vertex bbl = Vertex( QVector3D(-m_half_height, -m_half_width, -m_half_depth), CuboidVerts::red   );
    Vertex bbr = Vertex( QVector3D( m_half_height, -m_half_width, -m_half_depth), CuboidVerts::white );

    m_vertexArray = {
        // front
        ftr, ftl, fbl,
        fbl, fbr, ftr,
        // back
        bbr, btl, btr,
        btl, bbr, bbl,
        // top
        ftr, btr, btl,
        btl, ftl, ftr,
        // bottom
        fbr, fbl, bbl,
        bbl, bbr, fbr,
        // left
        fbl, ftl, btl,
        fbl, btl, bbl,
        // right
        ftr, fbr, bbr,
        bbr, btr, ftr
    };
}

std::array<Vertex, 36> &CuboidVerts::getVertexArray()
{
    return m_vertexArray;
}
