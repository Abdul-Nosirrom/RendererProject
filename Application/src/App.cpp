#include "App.h"

#include <sstream>

App::App() : m_Window(600, 400, "RomanceDawn")
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
    m_Window.GFX().DrawTestTriangle(m_Timer.Peek(),
        (m_Window.mouse.GetPosX()/300.f - 1.f), (1.f - m_Window.mouse.GetPosY()/200.f));
    m_Window.GFX().DrawTestTriangle(0.25f * m_Timer.Peek(), 0.f, 0.f);
    m_Window.GFX().SwapBuffer();
}
