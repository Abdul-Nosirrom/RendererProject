#include "App.h"

#include <sstream>

App::App() : m_Window(800, 300, "RomanceDawn")
{}

int App::Go()
{
    // Our message pump
    // Windows are event based, we have to respond to the events our window sends out otherwise the window hangs and does nothing as it waits for our response [GetMessage -> DispatchMessage]
    // lpfnWndProc: The window procedure is what defines the response to the window messages. We wanna create our own, but still send stuff to the DefWindowProc as there are many many messages,
    //				so we only wanna respond to the ones we care about, then let the default handle the rest.
    MSG msg;
    BOOL gResult;
    
    while ((gResult = GetMessage
    (
        &msg,			                        /* Message, out var message structure */
        nullptr,		                    /* HWND, we don't pass in ours because we wanna get all messages belonging to this application. In windows, everything is a window (button etc...), so by setting null here, we get everything belonging to us */
        0, 0			/* Range of messages we want. Setting both to 0 means we wanna get all messages */
    )) > 0)
    {
        TranslateMessage(&msg); // Primarily useful for WM_CHAR messages
        DispatchMessage(&msg);
        // Default window procedure will close the window on close, but not close the application. For the exact reason that it doesn't make assumptions on what window is being closed (everythings a window afterall)
        DoFrame();
    }

    if (gResult == -1)
    {
        throw RDWND_LAST_EXCEPT();
    }
	
    // Where our Exit Code goes to from PostQuitMessage, see WM_QUIT message
    return int(msg.wParam);
}

void App::DoFrame()
{
    if (m_Window.kbd.KeyIsPressed(VK_SPACE))
    {
        //MessageBox(nullptr, "Space Was Pressed!", "Key Notifier", MB_OKCANCEL | MB_ICONINFORMATION);
    }
    if (m_Window.mouse.IsInWindow())
    {
        std::ostringstream oss;
        oss << "Mouse Pos: (" << m_Window.mouse.GetPosX() << ", " << m_Window.mouse.GetPosY() << ")";
        m_Window.SetTitle(oss.str());
    }
}
