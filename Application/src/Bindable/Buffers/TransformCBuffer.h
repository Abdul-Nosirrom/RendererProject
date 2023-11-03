#pragma once
#include "ConstantBuffers.h"
#include "Drawable/Drawable.h"
#include "Utility/Maths.h"

class TransformCBuffer : public Bindable
{
public:
    TransformCBuffer( Graphics& gfx,const Drawable& parent );
    void Bind( Graphics& gfx ) noexcept override;
private:
    VertexConstantBuffer<Math::XMMATRIX> vcbuf;
    const Drawable& parent;
};
