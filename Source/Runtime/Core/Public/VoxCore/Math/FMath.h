#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <numbers>

#include "VoxCore/Core/Types/Common.h"

using FReal = float64;

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
    [[nodiscard]] static constexpr T Min3(T a, T b, T c) noexcept { return Min(Min(a, b), c); }

    template <typename T>
    [[nodiscard]] static constexpr T Max3(T a, T b, T c) noexcept { return Max(Max(a, b), c); }

    template <typename T>
    [[nodiscard]] static constexpr T Clamp(T value, T minValue, T maxValue) noexcept {
        return std::clamp(value, minValue, maxValue);
    }

    template <typename T>
    [[nodiscard]] static constexpr T Saturate(T value) noexcept {
        return Clamp(value, T(0), T(1));
    }

    template <typename T>
    [[nodiscard]] static constexpr T Abs(T value) noexcept { return value < T(0) ? -value : value; }

    template <typename T>
    [[nodiscard]] static constexpr T Sign(T value) noexcept { return value < T(0) ? T(-1) : T(1); }

    template <typename T>
    [[nodiscard]] static constexpr T Square(T value) noexcept { return value * value; }

    template <typename T>
    [[nodiscard]] static constexpr T Cube(T value) noexcept { return value * value * value; }

    template <typename T>
    [[nodiscard]] static constexpr T Lerp(const T& a, const T& b, FReal alpha) noexcept {
        return a + (b - a) * alpha;
    }

    template <typename T>
    [[nodiscard]] static constexpr T LerpStable(const T& a, const T& b, FReal alpha) noexcept {
        return (a * (1.0 - alpha)) + (b * alpha);
    }

    [[nodiscard]] static inline FReal Sqrt(FReal value) noexcept { return std::sqrt(value); }
    [[nodiscard]] static inline FReal InvSqrt(FReal value) noexcept { return 1.0 / std::sqrt(value); }
    [[nodiscard]] static inline FReal Pow(FReal value, FReal exponent) noexcept { return std::pow(value, exponent); }
    [[nodiscard]] static inline FReal Exp(FReal value) noexcept { return std::exp(value); }
    [[nodiscard]] static inline FReal Loge(FReal value) noexcept { return std::log(value); }
    [[nodiscard]] static inline FReal Log2(FReal value) noexcept { return std::log2(value); }
    [[nodiscard]] static inline FReal Sin(FReal radians) noexcept { return std::sin(radians); }
    [[nodiscard]] static inline FReal Cos(FReal radians) noexcept { return std::cos(radians); }
    [[nodiscard]] static inline FReal Tan(FReal radians) noexcept { return std::tan(radians); }
    [[nodiscard]] static inline FReal Asin(FReal value) noexcept { return std::asin(Clamp(value, -1.0, 1.0)); }
    [[nodiscard]] static inline FReal Acos(FReal value) noexcept { return std::acos(Clamp(value, -1.0, 1.0)); }
    [[nodiscard]] static inline FReal Atan2(FReal y, FReal x) noexcept { return std::atan2(y, x); }
    [[nodiscard]] static inline FReal Fmod(FReal x, FReal y) noexcept { return std::fmod(x, y); }
    [[nodiscard]] static inline FReal Floor(FReal value) noexcept { return std::floor(value); }
    [[nodiscard]] static inline FReal Ceil(FReal value) noexcept { return std::ceil(value); }
    [[nodiscard]] static inline FReal Round(FReal value) noexcept { return std::round(value); }

    [[nodiscard]] static inline int32 FloorToInt(FReal value) noexcept { return static_cast<int32>(Floor(value)); }
    [[nodiscard]] static inline int32 CeilToInt(FReal value) noexcept { return static_cast<int32>(Ceil(value)); }
    [[nodiscard]] static inline int32 RoundToInt(FReal value) noexcept { return static_cast<int32>(Round(value)); }

    static inline void SinCos(FReal* outSin, FReal* outCos, FReal radians) noexcept {
        const FReal sine = Sin(radians);
        const FReal cosine = Cos(radians);
        if (outSin != nullptr) {
            *outSin = sine;
        }
        if (outCos != nullptr) {
            *outCos = cosine;
        }
    }

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

    [[nodiscard]] static constexpr bool IsFinite(FReal value) noexcept {
        return value >= -std::numeric_limits<FReal>::max() && value <= std::numeric_limits<FReal>::max();
    }

    [[nodiscard]] static inline FReal ClampAxis(FReal angleDegrees) noexcept {
        FReal angle = Fmod(angleDegrees, 360.0);
        if (angle < 0.0) {
            angle += 360.0;
        }
        return angle;
    }

    [[nodiscard]] static inline FReal NormalizeAxis(FReal angleDegrees) noexcept {
        FReal angle = ClampAxis(angleDegrees);
        if (angle > 180.0) {
            angle -= 360.0;
        }
        return angle;
    }

    [[nodiscard]] static inline FReal UnwindDegrees(FReal angleDegrees) noexcept {
        return NormalizeAxis(angleDegrees);
    }
};
