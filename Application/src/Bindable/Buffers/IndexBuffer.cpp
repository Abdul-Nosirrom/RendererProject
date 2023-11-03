#include "IndexBuffer.h"

#include "../../Errors/GraphicsErrors.h"

IndexBuffer::IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices)
    : m_Count(UINT(indices.size()))
{
    INFOMAN(gfx);

    // Setup the index buffer description
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0u;
    ibd.MiscFlags = 0u;
    ibd.ByteWidth = UINT(m_Count * sizeof(unsigned short));
    ibd.StructureByteStride = sizeof(unsigned short);

    // Setup subresource that points to the buffer
    D3D11_SUBRESOURCE_DATA isd = {};
    isd.pSysMem = indices.data();

    // Create the buffer
    GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&ibd, &isd, &pIndexBuffer));
}

void IndexBuffer::Bind(Graphics& gfx) noexcept
{
    // Basic binding
    GetContext(gfx)->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
}

UINT IndexBuffer::GetCount() const noexcept
{
    return m_Count;
}
