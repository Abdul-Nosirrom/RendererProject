#include "Drawable.h"
#include "Bindable/Buffers/IndexBuffer.h"


void Drawable::Draw(Graphics& gfx) const noexcept(!IS_DEBUG)
{
    for (auto& Bindable : m_Binds)
    {
        Bindable->Bind(gfx);
    }

    // Call Draw
    gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
{
    m_Binds.push_back(std::move(bind));
}

void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> iBuffer) noexcept
{
    pIndexBuffer = iBuffer.get();
    m_Binds.push_back(std::move(iBuffer));
}
