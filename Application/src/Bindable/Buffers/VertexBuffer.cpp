#include "VertexBuffer.h"

#include "../../Errors/GraphicsErrors.h"
/*
template <class V>
VertexBuffer::VertexBuffer(Graphics& gfx, const std::vector<V>& vertices)
    : m_Stride(sizeof(V))
{
    INFOMAN(gfx);

    D3D11_BUFFER_DESC vbd;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.CPUAccessFlags = 0u;
    vbd.MiscFlags = 0u;
    vbd.ByteWidth = UINT(m_Stride * vertices.size());
    vbd.StructureByteStride = m_Stride;

    D3D11_SUBRESOURCE_DATA vsd;
    vsd.pSysMem = vertices.data();

    GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&vbd, &vsd, &pVertexBuffer));
}*/

void VertexBuffer::Bind(Graphics& gfx) noexcept
{
    const UINT offset = 0u;
    GetContext(gfx)->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &m_Stride, &offset);
}