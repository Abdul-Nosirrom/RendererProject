#include "Drawable.h"
#include "Bindable/Buffers/IndexBuffer.h"


void Drawable::Draw(Graphics& gfx) const noexcept(!IS_DEBUG)
{
    for (auto& Bindable : m_Binds)
    {
        Bindable->Bind(gfx);
    }
    for (auto& Bindable : GetStaticBinds())
    {
        Bindable->Bind(gfx);
    }

    // Call Draw
    gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
{
    assert("*MUST* use AddIndexBuffer to bind unique index buffer" && typeid(*bind) != typeid(IndexBuffer));
    m_Binds.push_back(std::move(bind));
}

void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> iBuffer) noexcept(!IS_DEBUG)
{
    assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
    pIndexBuffer = iBuffer.get();
    m_Binds.push_back(std::move(iBuffer));
}
