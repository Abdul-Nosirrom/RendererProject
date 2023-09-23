#pragma once
#include "RomanceException.h"
#include "RomanceWin.h"
#include "Keyboard.h"
#include "Mouse.h"

// NOTE: Don't throw exceptions in WinAPI/Callback functions, will just crash the program and will not allow us to retrieve error info as its windows that calls it not our program
// To do the above, we catch the exceptions in those WINAPI functions, store them, and them throw them later in our own functions
/* Error exception helper macros for windows */
#define RDWND_EXCEPT(hr) Window::Exception(__LINE__, __FILE__, hr) // useful macros for filling out line number and file location
#define RDWND_LAST_EXCEPT() Window::Exception(__LINE__, __FILE__, GetLastError())


/// @brief  Interface for the handle of the window we create and all its operations
class Window
{
public:
    /// @brief  Exception class for the window, handles Win32 HRESULT messages
    class Exception : public RomanceException
    {
    public:
        Exception(int line, const char* file, HRESULT hr) noexcept;
        char const* what() const override;
        const char* GetType() const noexcept override;
        static std::string TranslateErrorCode(HRESULT hr);
        HRESULT GetErrorCode() const noexcept;
        std::string GetErrorString() const noexcept;
    private:
        HRESULT m_hResult;
    };
private:
    /// @brief  Singleton manages registration/cleanup of window clas
    class WindowClass
    {
    public:
        static const char* GetName() noexcept;
        static HINSTANCE GetInstance() noexcept;
    private:
        WindowClass() noexcept;
        ~WindowClass();

        // Fun singleton handler stuff
        WindowClass(const WindowClass&) = delete;
        WindowClass& operator=(const WindowClass&) = delete;

        static constexpr const char* s_ClassName = "Romance Dawn DX11 Engine Window";
        static WindowClass s_WindowClass;
        HINSTANCE m_hInst;
    };

public:
    Window(int width, int height, const char* name);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    void SetTitle(const std::string& title);
    
private:
    // Cannot be a member function, Win32 API is not C++, no OOP so need a "free function"
    /// @brief  Job is to setup our window class instance with the window API, then switch proc function
    static LRESULT WINAPI HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

    /// @brief  Job is to invoke our member function
    static LRESULT WINAPI HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

    /// @brief  Member function that is piped with window messages
    LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

public:
    Keyboard kbd;
    Mouse mouse;
    
private:
    int m_Width;
    int m_Height;
    HWND m_hWnd;
};
