#pragma once
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
};
