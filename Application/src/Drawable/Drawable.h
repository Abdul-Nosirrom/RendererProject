#pragma once

#include <memory>

#include "Graphics.h"
#include "Utility/Maths.h"

class Drawable
{
    template<class T>
    friend class DrawableBase;
    
public:
    Drawable() = default;
    Drawable(const Drawable&) = delete;
    virtual Math::XMMATRIX GetTransformMat() const noexcept = 0;
    void Draw(Graphics& gfx) const noexcept(!IS_DEBUG);
    virtual void Update(float dT) noexcept = 0;

    void AddBind(std::unique_ptr<class Bindable> bind) noexcept(!IS_DEBUG);
    void AddIndexBuffer(std::unique_ptr<class IndexBuffer> iBuffer) noexcept(!IS_DEBUG);

    virtual ~Drawable() = default;

private:
    virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept = 0;

private:
    const IndexBuffer* pIndexBuffer = nullptr;
    std::vector<std::unique_ptr<Bindable>> m_Binds;
};
