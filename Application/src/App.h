#pragma once
#include "OdaTimer.h"
#include "Window.h"
#include "Bindable/Buffers/ConstantBuffers.h"

class App
{
public:
    App();
    ~App();
    
    /// @brief  Frame / Message loop
    int Go();

private:
    void DoFrame();

private:
    Window m_Window;
    OdaTimer m_Timer;
    std::vector<std::unique_ptr<class Box>> m_Boxes;
    std::unique_ptr<VertexConstantBuffer<Math::XMFLOAT4>> pTimeUniform;
    Math::XMFLOAT4 m_elapsedTime;
};
