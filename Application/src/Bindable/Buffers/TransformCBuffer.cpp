#include "TransformCBuffer.h"

TransformCBuffer::TransformCBuffer(Graphics& gfx, const Drawable& parent)
    : vcbuf(gfx),
        parent(parent)
{
}

void TransformCBuffer::Bind(Graphics& gfx) noexcept
{
    vcbuf.Update( gfx,
    Math::XMMatrixTranspose(
        parent.GetTransformMat() * gfx.GetProjectionMat()
    ));
    vcbuf.Bind( gfx );
}
