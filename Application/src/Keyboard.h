#pragma once
#include <bitset>
#include <queue>

class Keyboard
{
    friend class Window;

public:
    /// @brief  Internal class for handling keyboard event types
    class Event
    {
    public:
        enum class Type
        {
            Press,
            Release,
            Invalid
        };
    private:
        Type m_Type;
        unsigned char m_KeyCode;
    public:
        Event() noexcept : m_Type(Type::Invalid), m_KeyCode(0u) {}
        Event(Type type, unsigned char keycode) noexcept : m_Type(type), m_KeyCode(keycode) {}

        bool IsPress() const noexcept   { return m_Type == Type::Press; }
        bool IsRelease() const noexcept { return m_Type == Type::Release; }
        bool IsInvalid() const noexcept { return m_Type == Type::Invalid; }
    };

public:
    Keyboard() = default;
    Keyboard(const Keyboard&) = delete;
    Keyboard& operator=(const Keyboard&) = delete;

    /*--------------------------------------------------------------------------------------------------------------
	* Key Event Handlers
	*--------------------------------------------------------------------------------------------------------------*/
    bool KeyIsPressed(unsigned char keycode) const noexcept;
    Event ReadKey() noexcept;
    bool KeyIsEmpty() const noexcept;
    void FlushKey() noexcept;

    /*--------------------------------------------------------------------------------------------------------------
	* Char Event Handlers
	*--------------------------------------------------------------------------------------------------------------*/
    char ReadChar() noexcept;
    bool CharIsEmpty() const noexcept;
    void FlushChar() noexcept;
    void Flush() noexcept;

    /*--------------------------------------------------------------------------------------------------------------
	* Autorepeat Control
	*--------------------------------------------------------------------------------------------------------------*/
    void EnableAutoRepeat() noexcept;
    void DisableAutoRepeat() noexcept;
    bool AutoRepeatIsEnabled() const noexcept;

private:
    /*--------------------------------------------------------------------------------------------------------------
	* Internal message pump for windows procedure
	*--------------------------------------------------------------------------------------------------------------*/
    void OnKeyPressed(unsigned char keycode) noexcept;
    void OnKeyReleased(unsigned char keycode) noexcept;
    void OnChar(char character) noexcept;
    /// @brief  Clear key states, useful when losing focus for example
    void ClearState() noexcept;

    template<typename T>
    static void TrimBuffer(std::queue<T>& buffer) noexcept;

private:
    static constexpr  unsigned int  s_NumKeys = 256u;
    static constexpr unsigned int   s_BufferSize = 16u;
    bool                            m_bAutoRepeatEnabled = false;
    std::bitset<s_NumKeys>          m_KeyStates;
    std::queue<Event>               m_KeyBuffer;
    std::queue<char>                m_CharBuffer;
};
