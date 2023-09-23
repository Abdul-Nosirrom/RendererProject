#include "Mouse.h"
#include "RomanceWin.h"

std::pair<int, int> Mouse::GetPos() const noexcept
{
    return {m_xPos, m_yPos};
}

int Mouse::GetPosX() const noexcept
{
    return m_xPos;
}

int Mouse::GetPosY() const noexcept
{
    return m_yPos;
}

bool Mouse::IsInWindow() const noexcept
{
    return b_InWindow;
}

bool Mouse::LeftIsPressed() const noexcept
{
    return b_LeftPressed;
}

bool Mouse::RightIsPressed() const noexcept
{
    return b_RightPressed;
}

Mouse::Event Mouse::Read() noexcept
{
    if (!m_Buffer.empty())
    {
        Event e = m_Buffer.front();
        m_Buffer.pop();
        return e;
    }

    return Event();
}

void Mouse::Flush() noexcept
{
    m_Buffer = std::queue<Event>();
}

void Mouse::OnMouseMove(int x, int y) noexcept
{
    m_xPos = x;
    m_yPos = y;
    m_Buffer.push(Event(Event::Type::Move, *this));
    TrimBuffer();
}

void Mouse::OnMouseEnter() noexcept
{
    b_InWindow = true;
    m_Buffer.push(Event(Event::Type::Enter, *this));
    TrimBuffer();
}

void Mouse::OnMouseLeave() noexcept
{
    b_InWindow = false;
    m_Buffer.push(Event(Event::Type::Leave, *this));
    TrimBuffer();
}

void Mouse::OnLeftPressed(int x, int y) noexcept
{
    b_LeftPressed = true;
    m_Buffer.push(Event(Event::Type::LPress, *this));
    TrimBuffer();
}

void Mouse::OnLeftReleased(int x, int y) noexcept
{
    b_LeftPressed = false;
    m_Buffer.push(Event(Event::Type::RRelease, *this));
    TrimBuffer();
}

void Mouse::OnRightPressed(int x, int y) noexcept
{
    b_RightPressed = true;
    m_Buffer.push(Event(Event::Type::RPress, *this));
    TrimBuffer();
}

void Mouse::OnRightReleased(int x, int y) noexcept
{
    b_RightPressed = false;
    m_Buffer.push(Event(Event::Type::RRelease, *this));
    TrimBuffer();
}

void Mouse::OnWheelUp(int x, int y) noexcept
{
    m_Buffer.push(Event(Event::Type::WheelUp, *this));
    TrimBuffer();
}

void Mouse::OnWheelDown(int x, int y) noexcept
{
    m_Buffer.push(Event(Event::Type::WheelDown, *this));
    TrimBuffer();
}

void Mouse::OnWheelDelta(int x, int y, int delta) noexcept
{
    m_wheelDeltaCarry += delta;

    // Generate events for every 120 delta, according to MSD, keep value between [-WHEEL_DELTA, WHEEL_DELTA]
    while (m_wheelDeltaCarry >= WHEEL_DELTA)
    {
        m_wheelDeltaCarry -= WHEEL_DELTA;
        OnWheelUp(x, y);
    }
    while (m_wheelDeltaCarry <= -WHEEL_DELTA)
    {
        m_wheelDeltaCarry += WHEEL_DELTA;
        OnWheelDown(x, y);
    }
}

void Mouse::TrimBuffer() noexcept
{
    while (m_Buffer.size() > s_BufferSize)
    {
        m_Buffer.pop();
    }
}
