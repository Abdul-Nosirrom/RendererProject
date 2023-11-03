#pragma once
#include <memory>
#include <optional>

#include "Graphics.h"
#include "RomanceException.h"
#include "RomanceWin.h"
#include "Keyboard.h"
#include "Mouse.h"

/// @brief  Interface for the handle of the window we create and all its operations
class Window
{
public:
    /// @brief  Base exception class
    class Exception : RomanceException
    {
        using RomanceException::RomanceException;
    public:
        static std::string TranslateErrorCode(HRESULT hr);
    };
    
    /// @brief  Exception class for the window, handles Win32 HRESULT messages
    class HrException : public RomanceException
    {
    public:
        HrException(int line, const char* file, HRESULT hr) noexcept;
        char const* what() const override;
        const char* GetType() const noexcept override;
        HRESULT GetErrorCode() const noexcept;
        std::string GetErrorString() const noexcept;
    private:
        HRESULT m_hResult;
    };

    /// @brief  Exception to handle no graphics object creation
    class NoGFXException : public Exception
    {
    public:
        using Exception::Exception;
        const char* GetType() const noexcept override;
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

    /// @brief  Checks PeekMessage (for all windows, hence static), and handles it, returning a value if found an exit message
    static std::optional<int> ProcessMessages();

    Graphics& GFX();
    
private:
    // Cannot be a member function, Win32 API is not C++, no OOP so need a "free function"
    /// @brief  Job is to setup our window class instance with the window API, then switch proc function
    static LRESULT WINAPI HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

    /// @brief  Job is to invoke our member function
    static LRESULT WINAPI HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

    /// @brief  Member function that is piped with window messages
    LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

    void HandleWindowResizing(WPARAM wParam, LPARAM lParam);

public:
    Keyboard kbd;
    Mouse mouse;
    
private:
    int m_Width;
    int m_Height;
    HWND m_hWnd;
    std::unique_ptr<Graphics> pGFX;
};
