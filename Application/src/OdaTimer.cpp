#include "OdaTimer.h"

using namespace std::chrono;

OdaTimer::OdaTimer()
{
    last = steady_clock::now();
}

float OdaTimer::Mark()
{
    const auto old = last;
    last = steady_clock::now();
    const duration<float> frameTime = last - old;
    return frameTime.count();
}

float OdaTimer::Peek() const
{
    return duration<float>(steady_clock::now() - last).count();
}
