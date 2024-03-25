#pragma once
#include "IndexedTriangleList.h"
#include <array>

// NOTE: Winding number is important, unless culling is disabled in RasterizerState, will auto cull back faces

class Plane
{
public:
    template<class V>
    static IndexedTriangleList<V> MakeTesselated(int divisions_x, int divisions_y)
    {
        assert(divisions_x >= 1);
        assert(divisions_y >= 1);
        
        constexpr float width = 2.f;
        constexpr float height = 2.f;
        const int nVertices_x = divisions_x + 1;
        const int nVertices_y = divisions_y + 1;

        std::vector<V> vertices(nVertices_x * nVertices_y);

        constexpr float side_x = width / 2.f;
        constexpr float side_y = height / 2.f;
        const float divisionSize_x = width / float(divisions_x);
        const float divisionSize_y = height / float(divisions_y);
        const auto bottomLeft = Math::XMVectorSet(-side_x, -side_y, 0.f, 0.f);

        for (int y = 0, i = 0; y < nVertices_y; y++)
        {
            const float y_pos = float(y) * divisionSize_y;
            for (int x = 0; x < nVertices_x; x++, i++)
            {
                const auto v = Math::XMVectorAdd(bottomLeft, Math::XMVectorSet(float(x) * divisionSize_x, y_pos, 0.f, 0.f));
                Math::XMStoreFloat3(&vertices[i].pos, v);
            }
        }

        std::vector<unsigned short> indices;
        indices.reserve(Math::square(divisions_x * divisions_y) * 6);
        {
            // vertex to index lambda
            const auto vxy2i = [nVertices_x](size_t x, size_t y)
            {
                return (unsigned short)(y * nVertices_x + x);
            };

            for (size_t y = 0; y < divisions_y; y++)
            {
                for (size_t x = 0; x < divisions_x; x++)
                {
                    const std::array<unsigned short, 4> indexArray =
                    { vxy2i(x,y), vxy2i(x+1,y), vxy2i(x,y+1), vxy2i(x+1,y+1) };
                    indices.push_back(indexArray[0]);
                    indices.push_back(indexArray[2]);
                    indices.push_back(indexArray[1]);
                    indices.push_back(indexArray[1]);
                    indices.push_back(indexArray[2]);
                    indices.push_back(indexArray[3]);
                }
            }
        }

        return {std::move(vertices), std::move(indices)};
    }

    template<class V>
    static IndexedTriangleList<V> Make()
    {
        return MakeTesselated<V>(1, 1);
    }
};

class Cube
{
public:
    template<class V>
    static IndexedTriangleList<V> Make()
    {
        constexpr float Size = 1.f/2.f;
        std::vector<Math::XMFLOAT3> vertices =
        {
            {-Size, -Size,  -Size}, // 0
            {Size, -Size, -Size},   // 1
            {-Size,  Size, -Size},  // 2
            {Size,  Size, -Size},   // 3
            {-Size, -Size,  Size},  // 4
            {Size, -Size, Size},    // 5
            {-Size,  Size, Size},   // 6
            {Size,  Size, Size}     // 7
        };

        std::vector<V> verts(vertices.size());
        for (size_t i = 0; i < vertices.size(); i++)
        {
            verts[i].pos = vertices[i];
        }

        std::vector<unsigned short> indices =
        {
            // Front Face Quad (Fixed (-) Z)
            0, 2, 1,
            2, 3, 1,
            // Back Face Quad (Fixed (+) Z)
            4, 5, 7,
            4, 7, 6,
            // Right Face Quad (Fixed (+) X)
            1, 3, 5,
            3, 7, 5,
            // Left Face Quad (Fixed (-) X)
            0, 4, 2,
            2, 4, 6,
            // Top Face Quad (Fixed (+) Y)
            2, 6, 3,
            3, 6, 7,
            // Bottom Face Quad (Fixed (-) Y)
            0, 1, 4,
            1, 5, 4
        };

        return {std::move(verts), indices};
    }
};

class Sphere
{
public:
    template<class V>
    static IndexedTriangleList<V> MakeTesselated(int latDiv, int longDiv)
    {
        assert(latDiv >= 3);
        assert(longDiv >= 3);

        constexpr float radius = 1.f;
        const auto base = Math::XMVectorSet(0.f, 0.f, radius, 0.f);
        const float lattitudeAngle = Math::PI / latDiv;
        const float longitudeAngle = 2.f * Math::PI / longDiv;

        std::vector<V> vertices;
        for (int iLat = 1; iLat < latDiv; iLat++)
        {
            const auto latBase = Math::XMVector3Transform(base, Math::XMMatrixRotationX(lattitudeAngle * iLat));
            for (int iLong = 0; iLong < longDiv; iLong++)
            {
                vertices.emplace_back();
                auto v = Math::XMVector3Transform(latBase, Math::XMMatrixRotationZ(longitudeAngle * iLong));
                Math::XMStoreFloat3(&vertices.back().pos, v);
            }
        }

        // add the cap vertices
        const auto iNorthPole = (unsigned short)vertices.size();
        vertices.emplace_back();
        Math::XMStoreFloat3(&vertices.back().pos, base);
        const auto iSouthPole = (unsigned short)vertices.size();
        vertices.emplace_back();
        Math::XMStoreFloat3(&vertices.back().pos, Math::XMVectorNegate(base));

        const auto CalcIdx = [latDiv, longDiv](unsigned short iLat, unsigned short iLong){ return iLat * longDiv + iLong; };
        std::vector<unsigned short> indices;
        for (unsigned short iLat = 0; iLat < latDiv - 2; iLat++)
        {
            for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
            {
                // Tri 1
                indices.push_back(CalcIdx(iLat, iLong));
                indices.push_back(CalcIdx(iLat + 1, iLong));
                indices.push_back(CalcIdx(iLat, iLong + 1));
                // Tri 2
                indices.push_back(CalcIdx(iLat, iLong + 1));
                indices.push_back(CalcIdx(iLat + 1, iLong));
                indices.push_back(CalcIdx(iLat + 1, iLong + 1));
            }
            // Wrap band
            // Tri 1
            indices.push_back(CalcIdx(iLat, longDiv - 1));
            indices.push_back(CalcIdx(iLat + 1, longDiv - 1));
            indices.push_back(CalcIdx(iLat, 0));
            // Tri 2
            indices.push_back(CalcIdx(iLat, 0));
            indices.push_back(CalcIdx(iLat + 1, longDiv - 1));
            indices.push_back(CalcIdx(iLat + 1, 0));
        }

        // Cap fans
        for (unsigned short iLong = 0; iLong < longDiv; iLong++)
        {
            // North
            indices.push_back(iNorthPole);
            indices.push_back(CalcIdx(0, iLong));
            indices.push_back(CalcIdx(0, iLong+1));
            // South
            indices.push_back(CalcIdx(latDiv - 2, iLong+1));
            indices.push_back(CalcIdx(latDiv - 2, iLong));
            indices.push_back(iSouthPole);
        }

        // Wrap triangles
        // North
        indices.push_back(iNorthPole);
        indices.push_back(CalcIdx(0, longDiv-1));
        indices.push_back(CalcIdx(0,0));
        // South
        indices.push_back(CalcIdx(latDiv - 2, 0));
        indices.push_back(CalcIdx(latDiv - 2, longDiv - 1));
        indices.push_back(iSouthPole);

        return {std::move(vertices), std::move(indices)};
    }

    template<class V>
    static IndexedTriangleList<V> Make() { return MakeTesselated<V>(12, 24); }
};

class Cone
{
public:
    template<class V>
    static IndexedTriangleList<V> MakeTesselated(int longDiv)
    {
        assert(longDiv >= 3);

        const auto base = Math::XMVectorSet(1.f, 0.f, -1.f, 0.f);
        const float longitudeAngle = 2.f * Math::PI / longDiv;

        // base vertices
        std::vector<V> vertices;
        for (int iLong = 0; iLong < longDiv; iLong++)
        {
            vertices.emplace_back();
            auto v = Math::XMVector3Transform(
                base,
                Math::XMMatrixRotationZ(longitudeAngle * iLong));
            Math::XMStoreFloat3(&vertices.back().pos, v);
        }

        // the center
        vertices.emplace_back();
        vertices.back().pos = {0.f, 0.f, -1.f};
        const auto iCenter = (unsigned short)(vertices.size() - 1);
        // the tip
        vertices.emplace_back();
        vertices.back().pos = {0.f, 0.f, 1.f};
        const auto iTip = (unsigned short)(vertices.size() - 1);

        // base indices
        std::vector<unsigned short> indices;
        for (unsigned short iLong = 0; iLong < longDiv; iLong++)
        {
            indices.push_back(iCenter);
            indices.push_back((iLong + 1) % longDiv);
            indices.push_back(iLong);
        }

        // cone indices
        for (unsigned short iLong = 0; iLong < longDiv; iLong++)
        {
            indices.push_back(iLong);
            indices.push_back((iLong + 1) % longDiv);
            indices.push_back(iTip);
        }

        return {std::move(vertices), std::move(indices)};
    }

    template<class V>
    static IndexedTriangleList<V> Make()
    {
        return MakeTesselated<V>(24);
    }
};

/*
class Prism
{
public:
    template<class V>
    static IndexedTriangleList<V> MakeTesselated(int longDiv)
    {
        assert(longDiv >= 3);

    }
};
*/