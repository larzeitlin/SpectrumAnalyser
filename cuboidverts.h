#ifndef CUBOIDVERTS_H
#define CUBOIDVERTS_H

#include "vertex.h"
#include <array>
#include <QVector3D>

class CuboidVerts
{
public:
    CuboidVerts(float h, float w, float d);
    std::array<Vertex, 36> &getVertexArray();

private:
    float m_half_height;
    float m_half_width;
    float m_half_depth;
    std::array<Vertex, 36> m_vertexArray;
    static const QVector3D red;
    static const QVector3D blue;
    static const QVector3D green;
    static const QVector3D black;
    static const QVector3D white;
};
#endif // CUBOIDVERTS_H
