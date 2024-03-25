#include "TransformCBuffer.h"

TransformCBuffer::TransformCBuffer(Graphics& gfx, const Drawable& parent)
    : parent(parent)
{
    if (!pVcbuf)
    {
        pVcbuf = std::make_unique<VertexConstantBuffer<Math::XMMATRIX>>(gfx);
    }
}

void TransformCBuffer::Bind(Graphics& gfx) noexcept
{
    pVcbuf->Update( gfx,
    Math::XMMatrixTranspose(
        parent.GetTransformMat() * gfx.GetProjectionMat()
    ));
    pVcbuf->Bind( gfx );
}

std::unique_ptr<VertexConstantBuffer<Math::XMMATRIX>> TransformCBuffer::pVcbuf;