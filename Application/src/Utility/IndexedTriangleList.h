#pragma once
#include <vector>

#include "Maths.h"

template<class T>
class IndexedTriangleList
{
public:
    IndexedTriangleList() = default;
    IndexedTriangleList(std::vector<T> verts_in, std::vector<unsigned short> indices_in)
        : m_Vertices(verts_in), m_Indices(indices_in)
    {
        assert(m_Vertices.size() > 2);
        assert(m_Indices.size() % 3 == 0);
    }

    void Transform(Math::FXMMATRIX matrix)
    {
        for (auto& v : m_Vertices)
        {
            const Math::XMVECTOR pos = Math::XMLoadFloat3(&v.pos);
            Math::XMStoreFloat3(&v.pos, Math::XMVector3Transform(pos, matrix));
        }
    }
    
public:
    std::vector<T> m_Vertices;
    std::vector<unsigned short> m_Indices;
};
