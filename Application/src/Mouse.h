#pragma once
#include <queue>

class Mouse
{
    friend class Window;
public:
    /// @brief  Internal class for handling mouse event types
    class Event
    {
    public:
        enum class Type
        {
            LPress,
            LRelease,
            RPress,
            RRelease,
            WheelUp,
            WheelDown,
            Move,
            Enter,
            Leave,
            Invalid
        };
    private:
        Type m_Type;
        bool b_LeftPressed;
        bool b_RightPressed;
        int m_xPos, m_yPos;
    public:
        Event() noexcept : m_Type(Type::Invalid), b_LeftPressed(false), b_RightPressed(false), m_xPos(0), m_yPos(0) {}
        Event(Type type, const Mouse& parent) noexcept 
            : m_Type(type), b_LeftPressed(parent.b_LeftPressed), b_RightPressed(parent.b_RightPressed), m_xPos(parent.m_xPos), m_yPos(parent.m_yPos) {}

        bool IsValid() const noexcept
        {
            return m_Type != Type::Invalid;
        }

        Type GetType() const noexcept
        {
            return m_Type;
        }

        int GetPosX() const noexcept { return m_xPos; }
        int GetPosY() const noexcept { return m_yPos; }

        bool LeftIsPressed() const noexcept { return b_LeftPressed; }
        bool RightIsPressed() const noexcept { return b_RightPressed; }
    };

public:
    Mouse() = default;
    Mouse(const Mouse&) = delete;
    Mouse& operator=(const Mouse&) = delete;

    std::pair<int, int> GetPos() const noexcept;
    int GetPosX() const noexcept;
    int GetPosY() const noexcept;
    bool IsInWindow() const noexcept;
    bool LeftIsPressed() const noexcept;
    bool RightIsPressed() const noexcept;

    Event Read() noexcept;
    bool IsEmpty() const noexcept
    {
        return m_Buffer.empty();
    }
    void Flush() noexcept;

private:
    void OnMouseMove(int x, int y) noexcept;
    void OnMouseEnter() noexcept;
    void OnMouseLeave() noexcept;
    void OnLeftPressed(int x, int y) noexcept;
    void OnLeftReleased(int x, int y) noexcept;
    void OnRightPressed(int x, int y) noexcept;
    void OnRightReleased(int x, int y) noexcept;
    void OnWheelUp(int x, int y) noexcept;
    void OnWheelDown(int x, int y) noexcept;
    void OnWheelDelta(int x, int y, int delta) noexcept;
    void TrimBuffer() noexcept;

private:
    static constexpr unsigned int s_BufferSize = 16u;
    int m_xPos, m_yPos;
    int m_wheelDeltaCarry   = 0;
    bool b_LeftPressed      = false;
    bool b_RightPressed     = false;
    bool b_InWindow         = false;
    std::queue<Event> m_Buffer;
};
