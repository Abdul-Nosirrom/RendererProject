#include "Window.h"

#include <iostream>
#include <sstream>

#include "resource.h"
#include "WindowsMessageMap.h"
#include "Errors/ErrorUtilities.h"
#include "Errors/WindowErrors.h"

/*--------------------------------------------------------------------------------------------------------------
* Exception Class
*--------------------------------------------------------------------------------------------------------------*/

Window::HrException::HrException(int line, const char* file, HRESULT hr) noexcept
    : RomanceException(line, file), m_hResult(hr)
{}

char const* Window::HrException::what() const
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "[Error Code] " << GetErrorCode() << std::endl
        << "[Description] " << HRErrors::TranslateErrorCode(m_hResult) << std::endl
        << GetOriginString();
    m_whatBuffer = oss.str();
    return m_whatBuffer.c_str();
}

const char* Window::HrException::GetType() const noexcept
{
    return RomanceException::GetType();
}


HRESULT Window::HrException::GetErrorCode() const noexcept
{
    return m_hResult;
}

std::string Window::HrException::GetErrorString() const noexcept
{
    return HRErrors::TranslateErrorCode(m_hResult);
}

const char* Window::NoGFXException::GetType() const noexcept
{
    return "Romance Exception [NO GFX]";
}

/*--------------------------------------------------------------------------------------------------------------
* Window Class Singleton
*--------------------------------------------------------------------------------------------------------------*/

/* Initialize Static Member */
Window::WindowClass Window::WindowClass::s_WindowClass;


Window::WindowClass::WindowClass() noexcept
    : m_hInst(GetModuleHandle(nullptr)) // NOTE: This is a way to get the handle instance without having to pass it through from the entrypoint, loads on EXE startup which is when the singleton is created, hence no manual creation
{
    // Setup Registration Info Struct & Register window class
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(wc);         	/* Must specify size of struct we're passing in */
    wc.style = CS_OWNDC;				/* Allocates a unique device context for each window in the class */
    wc.lpfnWndProc = HandleMsgSetup;	/* Windows Proc Func Pointer defining how we respond to window messages in the message pump loop (Win spits out messages in a qeue for us to process and send back  */
    wc.cbClsExtra = 0;					/* Extra bites on the class size */
    wc.cbWndExtra = 0;					/* Extra bites on the window */
    wc.hInstance = GetInstance();		/* The HInstance we got */
    wc.hIcon = static_cast<HICON>(LoadImage(m_hInst, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 32, 32, 0));
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = GetName();
    wc.hIconSm = static_cast<HICON>(LoadImage(m_hInst, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 32, 32, 0));;
    RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
    // Window Unregistration Call
    UnregisterClass(s_ClassName, GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
    return s_ClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
    return s_WindowClass.m_hInst;
}

/*--------------------------------------------------------------------------------------------------------------
* Window Interface
*--------------------------------------------------------------------------------------------------------------*/

Window::Window(int width, int height, const char* name)
    : m_Width(width), m_Height(height)
{
    // Window Rect Initial (AdjustWindowRect is another method of stylizing our window, this is based on the 'client region' instead of the whole
    // window region (the area in-between the borders)
    RECT wr;
    wr.left = 100;
    wr.right = width + wr.left;
    wr.top = 100;
    wr.bottom = height + wr.top;

    // Check if everything's alright (Return value is 0 if it failed according to documentation)
    if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
    {
        throw RDWND_LAST_EXCEPT();   
    }

    // Create window and get window handle
    m_hWnd = CreateWindow(
        WindowClass::GetName(),						/* Class Name that was registered! */
        name,									    /* Window Name */
        WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_SIZEBOX,	/* Window Style*/
        CW_USEDEFAULT, CW_USEDEFAULT,				/* Initial Window Position (Macro for "Default") */
        wr.right - wr.left, wr.bottom - wr.top,     /* Window Size */
        nullptr, nullptr,                           /* Parent & Menu window we don't have, set them to null*/
        WindowClass::GetInstance(),                 /* HInstance */
        this                                        /* Long pointer param, passed through our windowProc as the lParam during WM_NCCREATE */
    );

    // Check if window was successfully created
    if (m_hWnd == nullptr)
    {
        throw RDWND_LAST_EXCEPT();    
    }
    
    // Show the window
    ShowWindow(m_hWnd, SW_SHOWDEFAULT);

    // Create the graphics object
    pGFX = std::make_unique<Graphics>(m_hWnd);
}

Window::~Window()
{
    // Destroy the window from its handle
    DestroyWindow(m_hWnd);
}

void Window::SetTitle(const std::string& title)
{
    if (SetWindowText(m_hWnd, title.c_str()) == 0)
    {
        throw RDWND_LAST_EXCEPT();
    }
}

std::optional<int> Window::ProcessMessages()
{
    // Our message pump
    // Windows are event based, we have to respond to the events our window sends out otherwise the window hangs and does nothing as it waits for our response [GetMessage -> DispatchMessage]
    // lpfnWndProc: The window procedure is what defines the response to the window messages. We wanna create our own, but still send stuff to the DefWindowProc as there are many many messages,
    //				so we only wanna respond to the ones we care about, then let the default handle the rest.
    MSG msg;
    
    while (PeekMessage
    (
        &msg,			                        /* Message, out var message structure */
        nullptr,		                    /* HWND, we don't pass in ours because we wanna get all messages belonging to this application. In windows, everything is a window (button etc...), so by setting null here, we get everything belonging to us */
        0, 0, 			/* Range of messages we want. Setting both to 0 means we wanna get all messages */
        PM_REMOVE                   /* How to deal with the message after peeking, in this case, remove it from the message queue */
    ))
    {
        // Check for quit because PeekMessage does not signal this via return values, only if it gets a message
        // GetMessages return value does signal if it was a quit message (hence we did while(GetMessage) in the app loop)
        if (msg.message == WM_QUIT)
        {
            // return the optional wrapping int (arg to PostQuitMessage is in wParam)
            return int(msg.wParam);
        }
        
        TranslateMessage(&msg); // Primarily useful for WM_CHAR messages
        DispatchMessage(&msg);
        // Default window procedure will close the window on close, but not close the application. For the exact reason that it doesn't make assumptions on what window is being closed (everythings a window afterall)
    }

    // Return empty optional if no message was retrieved this frame
    return {};
}

Graphics& Window::GFX()
{
    if (!pGFX)
    {
        throw RDWND_NOGFX_EXCEPT();
    }
    return *pGFX;
}

LRESULT Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    // Even though this is a static function, we can access the window instance we created via the lParam during WM_NCCREATE
    if (msg == WM_NCCREATE)
    {
        // Extract ptr to window class from creation data
        const CREATESTRUCTW* pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
        // Set WinAPI-Managed user data to store ptr to window class
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));

        {
            // Example usage to access our window class instance via the window API side based on SetWindowLongPtr
            //GetWindowLongPtr(hWnd, GWLP_USERDATA);
            // This way we can now pipe all our window messages to our member function
        }
        
        // Set message proc to normal (non-setup) handle now that setup is finished, we go through MsgThunk
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
        // forward message to window class handler
        return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    // Retrieve ptr to window class and pipe the message to it
    Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    //static WindowsMessageMap messageMap;
    //OutputDebugString(messageMap(msg, lParam, wParam).c_str());

    static bool b_Resizing = false;

    switch( msg )
    {
        // NOTE: We don't want DefProc to handle this because we want our destructor to destroy the window, so return 0
        case WM_CLOSE:
            PostQuitMessage( 0 );
            return 0;

        // NOTE: Clear up states on lose focus to avoid having "zombie keys" floating around
        case WM_KILLFOCUS:
            kbd.ClearState();
            break;

        // Resizing the window, invoke event on Graphics to adjust viewport
        case WM_SIZE:
            HandleWindowResizing(wParam, lParam);
            break;
        ////////////////////////////////
        /***** KEYBOARD MESSAGES *****/
        case WM_SYSKEYDOWN: // NOTE: syskey commands need to be handled to track ALT key and other system keys like F10 (VK_MENU)
        case WM_KEYDOWN:
            // [https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-keydown] Bit 30 we wanna check for autorepeat sequences, its the "previous key state"
            if (!(lParam & 0x40000000) || kbd.AutoRepeatIsEnabled())
            {
                kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
            }
            break;
        case WM_SYSKEYUP:
        case WM_KEYUP:
            kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
            break;
        case WM_CHAR:
            kbd.OnChar(static_cast<unsigned char>(wParam));
            break;
        /***** END KEYBOARD MESSAGES *****/
        //////////////////////////////////
        /***** MOUSE MESSAGES *****/
        case WM_MOUSEMOVE:
            {
                const POINTS pt = MAKEPOINTS(lParam);
                // In client region, log move and log enter + capture mouse (if previously out of region)
                if (pt.x >= 0 && pt.x < m_Width && pt.y >= 0 && pt.y < m_Height)
                {
                    mouse.OnMouseMove(pt.x, pt.y);
                    if (!mouse.IsInWindow())
                    {
                        // Win API to capture mouse, keep it captured even
                        SetCapture(m_hWnd);
                        mouse.OnMouseEnter();
                    }
                }
                // Not in client region, log move/ maintain capture iff button down
                else
                {
                    // if statement equivalent to [wParam & (MK_LBUTTON | MK_RBUTTON)]
                    if (mouse.LeftIsPressed() || mouse.RightIsPressed())
                    {
                        mouse.OnMouseMove(pt.x, pt.y); // e.g this scope can happen when dragging the window for example
                    }
                    // Button up, release capture / log event for leaving
                    else
                    {
                        ReleaseCapture();
                        mouse.OnMouseLeave();
                    }
                }
                break;
            }
        case WM_LBUTTONDOWN:
            {
                const POINTS pt = MAKEPOINTS(lParam);
                mouse.OnLeftPressed(pt.x, pt.y);
                break;
            }
        case WM_LBUTTONUP:
            {
                const POINTS pt = MAKEPOINTS(lParam);
                mouse.OnLeftReleased(pt.x, pt.y);
                // Release mouse if input released outside of client region
                if (pt.x < 0 || pt.x >= m_Width || pt.y < 0 || pt.y >= m_Height)
                {
                    ReleaseCapture();
                    mouse.OnMouseLeave();
                }
                break;   
            }
        case WM_RBUTTONDOWN:
            {
                const POINTS pt = MAKEPOINTS(lParam);
                mouse.OnRightPressed(pt.x, pt.y);
                break;
            }
        case WM_RBUTTONUP:
            {
                const POINTS pt = MAKEPOINTS(lParam);
                mouse.OnRightReleased(pt.x, pt.y);
                // Release mouse if input released outside of client region
                if (pt.x < 0 || pt.x >= m_Width || pt.y < 0 || pt.y >= m_Height)
                {
                    ReleaseCapture();
                    mouse.OnMouseLeave();
                }
                break;
            }
        case WM_MOUSEWHEEL:
            {
                const POINTS pt = MAKEPOINTS(lParam);
                const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
                mouse.OnWheelDelta(pt.x, pt.y, delta);
                break;
            }
        case WM_MOUSELEAVE: // Invalidate our mouse here
            {
                mouse.OnMouseLeave();
                break;
            }
        /***** END MOUSE MESSAGES *****/
    }

    return DefWindowProc( hWnd,msg,wParam,lParam );
    //std::cout << messageMap(msg, lParam, wParam).c_str() << std::endl;
	
    switch (msg)
    {
        case WM_CLOSE:
            PostQuitMessage(1); /* Posts a quit message w/ our input exit code to the message queue */
        break;
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE)
                PostQuitMessage(2);
            else if (wParam == 'F') // Can use literals
                SetWindowText(m_hWnd, "AY LOOK!"); // Change window title
        break;
        case WM_CHAR: // Character input, key down happens before WM_CHAR, so WM_CHAR will always overwrite it (hence we dont get AY LOOK)
            {
                static std::string title;
                title.push_back((char)wParam);
                SetWindowText(m_hWnd, title.c_str());
            }
        break;
        case WM_LBUTTONDOWN:
            const POINTS pt = MAKEPOINTS(lParam); // Get mouse click coord points
        std::ostringstream oss;
        oss << "(" << pt.x << ", " << pt.y << ")";
        SetWindowText(m_hWnd, oss.str().c_str());
    }
    return DefWindowProc(m_hWnd, msg, wParam, lParam);
}

void Window::HandleWindowResizing(WPARAM wParam, LPARAM lParam)
{
    RECT rectp;// = (PRECT)lParam;

    // Get the window and client dimensions
    GetClientRect(m_hWnd, &rectp);

    m_Height = rectp.bottom - rectp.top;
    m_Width = rectp.right - rectp.left;

    if (pGFX)
        pGFX->OnViewPortUpdate(float(m_Width), float(m_Height));
}

