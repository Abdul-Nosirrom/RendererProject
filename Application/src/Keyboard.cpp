#include "Keyboard.h"

/*--------------------------------------------------------------------------------------------------------------
* Key Events
*--------------------------------------------------------------------------------------------------------------*/

bool Keyboard::KeyIsPressed(unsigned char keycode) const noexcept
{
    return m_KeyStates[keycode];
}

Keyboard::Event Keyboard::ReadKey() noexcept
{
    if (!m_KeyBuffer.empty())
    {
        Keyboard::Event e = m_KeyBuffer.front();
        m_KeyBuffer.pop();
        return e;
    }

    // Otherwise buffer isn't initialize, return empty event which is in an invalid state
    return Keyboard::Event();
}

bool Keyboard::KeyIsEmpty() const noexcept
{
    return m_KeyBuffer.empty();
}

void Keyboard::FlushKey() noexcept
{
    m_KeyBuffer = std::queue<Event>();
}

/*--------------------------------------------------------------------------------------------------------------
* Char Events
*--------------------------------------------------------------------------------------------------------------*/

char Keyboard::ReadChar() noexcept
{
    if (!m_CharBuffer.empty())
    {
        unsigned char charcode = m_CharBuffer.front();
        m_CharBuffer.pop();
        return charcode;
    }

    return 0;
}

bool Keyboard::CharIsEmpty() const noexcept
{
    return m_CharBuffer.empty();
}

void Keyboard::FlushChar() noexcept
{
    m_CharBuffer = std::queue<char>();
}

void Keyboard::Flush() noexcept
{
    FlushChar();
    FlushKey();
}

/*--------------------------------------------------------------------------------------------------------------
* AutoRepeat Control
*--------------------------------------------------------------------------------------------------------------*/


void Keyboard::EnableAutoRepeat() noexcept
{
    m_bAutoRepeatEnabled = true;
}

void Keyboard::DisableAutoRepeat() noexcept
{
    m_bAutoRepeatEnabled = false;
}

bool Keyboard::AutoRepeatIsEnabled() const noexcept
{
    return m_bAutoRepeatEnabled;
}

/*--------------------------------------------------------------------------------------------------------------
* Windows Proc Message Pump Handlers
*--------------------------------------------------------------------------------------------------------------*/


void Keyboard::OnKeyPressed(unsigned char keycode) noexcept
{
    m_KeyStates[keycode] = true;
    m_KeyBuffer.push(Event(Event::Type::Press, keycode));
    TrimBuffer(m_KeyBuffer); // Keep our buffer size fixed
}

void Keyboard::OnKeyReleased(unsigned char keycode) noexcept
{
    m_KeyStates[keycode] = false;
    m_KeyBuffer.push(Event(Event::Type::Release, keycode));
    TrimBuffer(m_KeyBuffer); // Keep our buffer size fixed
}

void Keyboard::OnChar(char character) noexcept
{
    m_CharBuffer.push(character);
    TrimBuffer(m_CharBuffer); // Keep our buffer size fixed
}

void Keyboard::ClearState() noexcept
{
    m_KeyStates.reset();
}

template <typename T>
void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept
{
    while (buffer.size() > s_BufferSize)
    {
        buffer.pop();
    }
}
