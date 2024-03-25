#pragma once
#include <DirectXMath.h>


namespace Math
{
    constexpr float PI = 3.14159265f;
    constexpr double PI_D = 3.1415926535897932;
    
    using namespace DirectX;

    template<typename T>
    constexpr T square(const T& x) { return x * x; }

    template<typename T>
    constexpr T lerp(const T& a, const T& b, float alpha)
    {
        return a * (1-alpha) + b * alpha;
    }

    template<typename T>
    constexpr T to_rad(T deg) { return deg * PI / (T)180.0; }

    template<typename T>
    constexpr T wrap_angle(T theta)
    {
        const T modded = fmod(theta, (T)2.0 * (T)PI_D);
        return (modded > (T)PI_D) ? (modded - (T)2.0 * (T)PI_D) : modded;
    }
}
