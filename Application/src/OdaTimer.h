#pragma once
#include <chrono>

class OdaTimer
{
public:

    OdaTimer();
    /// @brief  Returns the time elapsed since the last time Mark() was called, then updates mark point
    float Mark();
    /// @brief  Returns the time passed since the last time Mark() was called without resetting the mark point
    float Peek() const;

private:
    std::chrono::steady_clock::time_point last;
};
