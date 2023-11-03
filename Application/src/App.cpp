#include "App.h"

#include <iostream>
#include <random>
#include <sstream>

#include "Drawable/Box.h"

App::App() : m_Window(800, 600, "RomanceDawn")
{
    std::mt19937 rng( std::random_device{}() );
    std::uniform_real_distribution<float> adist( 0.0f,3.1415f * 2.0f );
    std::uniform_real_distribution<float> ddist( 0.0f,3.1415f * 2.0f );
    std::uniform_real_distribution<float> odist( 0.0f,3.1415f * 0.3f );
    std::uniform_real_distribution<float> rdist( 6.0f,20.0f );
    for( auto i = 0; i < 48; i++ )
    {
        m_Boxes.push_back( std::make_unique<Box>(
            m_Window.GFX(),rng,adist,
            ddist,odist,rdist
        ) );
    }
    m_Window.GFX().SetProjectionMat( Math::XMMatrixPerspectiveLH( 1.0f,3.0f / 4.0f,0.5f,40.0f ) );
}

App::~App()
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
    m_Window.GFX().ClearBuffer(.5f, 0.5f, 0.5f);

    const auto dT = m_Timer.Mark();
    for (auto &d : m_Boxes)
    {
        d->Update(dT);
        d->Draw(m_Window.GFX());
    }

    m_Window.GFX().SwapBuffer();
}
