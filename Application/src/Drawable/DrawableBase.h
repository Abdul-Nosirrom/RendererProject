#pragma once
#include "Drawable.h"
#include "Bindable/BindableCommon.h"

template<typename T>
class DrawableBase : public Drawable
{
protected:
    bool IsStaticInitialized() const noexcept { return !s_Bindables.empty(); }
    
    void AddSharedBindable(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
    {
        assert("*MUST* use AddSharedIndexBuffer to bind shared index buffer" && typeid(*bind) != typeid(IndexBuffer));
        s_Bindables.push_back(std::move(bind));   
    }
    void AddSharedIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noexcept(!IS_DEBUG)
    {
        assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
        pIndexBuffer = ibuf.get();
        s_Bindables.push_back(std::move(ibuf));
    }
    void SetIndexBufferFromSharedBindables() noexcept
    {
        assert("Attempting to set index buffer when it already exists" && pIndexBuffer == nullptr);
        for (auto& Bindable : s_Bindables)
        {
            if (auto ibuf = dynamic_cast<IndexBuffer*>(Bindable.get()))
            {
                pIndexBuffer = ibuf;
                return;
            }
        }
        assert("Failed to find index buffer in static binds" && pIndexBuffer != nullptr);
    }

private:
    
    const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override { return s_Bindables; }
    
private:
    static std::vector<std::unique_ptr<Bindable>> s_Bindables;
};

template<typename T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::s_Bindables;