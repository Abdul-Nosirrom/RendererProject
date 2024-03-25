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
    static std::unique_ptr<VertexConstantBuffer<Math::XMMATRIX>> pVcbuf;
    const Drawable& parent;
};
