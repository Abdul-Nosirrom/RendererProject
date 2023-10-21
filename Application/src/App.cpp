#include "App.h"

#include <sstream>

App::App() : m_Window(800, 300, "RomanceDawn")
{}

int App::Go()
{
    while (true)
    {
        // Process all messages pending
        if (const auto ecode = Window::ProcessMessages())
        {
            // if return optional has a value, that means its an exit code
            return *ecode;
        }

        DoFrame();
    }
}

void App::DoFrame()
{
    // Present frame
    m_Window.GFX().ClearBuffer(0.f, 0.f, 0.f);
    m_Window.GFX().DrawTestTriangle(m_Timer.Peek());
    m_Window.GFX().SwapBuffer();
}
