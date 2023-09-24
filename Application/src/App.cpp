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
    const float t = m_Timer.Peek();
    std::ostringstream oss;
    oss << "Time Elapsed: " << t << std::endl;
    m_Window.SetTitle(oss.str());
}
