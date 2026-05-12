//
// Created by IDKTHIS on 12.05.2026.
//

#pragma once

#include <ostream>

#include "FMath.h"
#include "Vector.h"

struct FRotator {
    FReal Pitch = 0.0f;
    FReal Yaw = 0.0f;
    FReal Roll = 0.0f;

    constexpr FRotator() = default;
    constexpr FRotator(FReal pitch, FReal yaw, FReal roll) : Pitch(pitch), Yaw(yaw), Roll(roll) {}

    [[nodiscard]] static constexpr FRotator ZeroRotator() noexcept { return {}; }

    [[nodiscard]] constexpr FRotator operator+(const FRotator& rhs) const noexcept {
        return {Pitch + rhs.Pitch, Yaw + rhs.Yaw, Roll + rhs.Roll};
    }

    [[nodiscard]] constexpr FRotator operator-(const FRotator& rhs) const noexcept {
        return {Pitch - rhs.Pitch, Yaw - rhs.Yaw, Roll - rhs.Roll};
    }

    [[nodiscard]] constexpr FRotator operator*(FReal scalar) const noexcept {
        return {Pitch * scalar, Yaw * scalar, Roll * scalar};
    }

    FRotator& Normalize() noexcept {
        Pitch = FMath::NormalizeAxis(Pitch);
        Yaw = FMath::NormalizeAxis(Yaw);
        Roll = FMath::NormalizeAxis(Roll);
        return *this;
    }

    [[nodiscard]] FRotator GetNormalized() const noexcept {
        FRotator copy(*this);
        return copy.Normalize();
    }

    [[nodiscard]] bool IsNearlyZero(FReal tolerance = FMath::KindaSmallNumber) const noexcept {
        return FMath::IsNearlyZero(Pitch, tolerance)
            && FMath::IsNearlyZero(Yaw, tolerance)
            && FMath::IsNearlyZero(Roll, tolerance);
    }

    [[nodiscard]] FVector Euler() const noexcept { return {Pitch, Yaw, Roll}; }

    [[nodiscard]] FVector Vector() const noexcept {
        const FReal pitchRadians = FMath::DegreesToRadiansFn(Pitch);
        const FReal yawRadians = FMath::DegreesToRadiansFn(Yaw);
        const FReal cosPitch = FMath::Cos(pitchRadians);
        return {
            cosPitch * FMath::Cos(yawRadians),
            cosPitch * FMath::Sin(yawRadians),
            FMath::Sin(pitchRadians)};
    }
};

inline std::ostream& operator<<(std::ostream& stream, const FRotator& value) {
    stream << "(Pitch=" << value.Pitch << ", Yaw=" << value.Yaw << ", Roll=" << value.Roll << ")";
    return stream;
}
