//
// Created by IDKTHIS on 12.05.2026.
//

#pragma once
#include <algorithm>
#include <cmath>
#include <limits>

using FReal = double;

struct FMath final {
    static constexpr FReal Pi = std::numbers::pi_v<FReal>;
    static constexpr FReal HalfPi = Pi * 0.5;
    static constexpr FReal TwoPi = Pi * 2.0;
    static constexpr FReal SmallNumber = 1e-8;
    static constexpr FReal KindaSmallNumber = 1e-4;
    static constexpr FReal InvPi = 1.0 / Pi;
    static constexpr FReal DegreesToRadians = Pi / 180.0;
    static constexpr FReal RadiansToDegrees = 180.0 / Pi;

    template <typename T>
    [[nodiscard]] static constexpr T Min(T a, T b) noexcept { return a < b ? a : b; }

    template <typename T>
    [[nodiscard]] static constexpr T Max(T a, T b) noexcept { return a > b ? a : b; }

    template <typename T>
    [[nodiscard]] static constexpr T Clamp(T value, T minValue, T maxValue) noexcept {
        return std::clamp(value, minValue, maxValue);
    }

    template <typename T>
    [[nodiscard]] static constexpr T Abs(T value) noexcept { return value < T(0) ? -value : value; }

    template <typename T>
    [[nodiscard]] static constexpr T Square(T value) noexcept { return value * value; }

    template <typename T>
    [[nodiscard]] static constexpr T Lerp(const T& a, const T& b, FReal alpha) noexcept {
        return a + (b - a) * alpha;
    }

    [[nodiscard]] static inline FReal Sqrt(FReal value) noexcept { return std::sqrt(value); }
    [[nodiscard]] static inline FReal InvSqrt(FReal value) noexcept { return 1.0f / std::sqrt(value); }
    [[nodiscard]] static inline FReal Sin(FReal radians) noexcept { return std::sin(radians); }
    [[nodiscard]] static inline FReal Cos(FReal radians) noexcept { return std::cos(radians); }
    [[nodiscard]] static inline FReal Tan(FReal radians) noexcept { return std::tan(radians); }
    [[nodiscard]] static inline FReal Asin(FReal value) noexcept { return std::asin(value); }
    [[nodiscard]] static inline FReal Acos(FReal value) noexcept { return std::acos(value); }
    [[nodiscard]] static inline FReal Atan2(FReal y, FReal x) noexcept { return std::atan2(y, x); }
    [[nodiscard]] static inline FReal Fmod(FReal x, FReal y) noexcept { return std::fmod(x, y); }

    [[nodiscard]] static constexpr FReal DegreesToRadiansFn(FReal degrees) noexcept {
        return degrees * DegreesToRadians;
    }

    [[nodiscard]] static constexpr FReal RadiansToDegreesFn(FReal radians) noexcept {
        return radians * RadiansToDegrees;
    }

    [[nodiscard]] static constexpr bool IsNearlyZero(
        FReal value,
        FReal tolerance = KindaSmallNumber) noexcept {
        return Abs(value) <= tolerance;
    }

    [[nodiscard]] static constexpr bool IsNearlyEqual(
        FReal a,
        FReal b,
        FReal tolerance = KindaSmallNumber) noexcept {
        return Abs(a - b) <= tolerance;
    }

    [[nodiscard]] static inline FReal ClampAxis(FReal angleDegrees) noexcept {
        FReal angle = Fmod(angleDegrees, 360.0f);
        if (angle < 0.0f) {
            angle += 360.0f;
        }
        return angle;
    }

    [[nodiscard]] static inline FReal NormalizeAxis(FReal angleDegrees) noexcept {
        FReal angle = ClampAxis(angleDegrees);
        if (angle > 180.0f) {
            angle -= 360.0f;
        }
        return angle;
    }
};