#pragma once
#include "OdaTimer.h"
#include "Window.h"

class App
{
public:
    App();

    /// @brief  Frame / Message loop
    int Go();

private:
    void DoFrame();

private:
    Window m_Window;
    OdaTimer m_Timer;
};
