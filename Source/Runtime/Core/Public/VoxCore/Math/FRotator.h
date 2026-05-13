#pragma once

#include <ostream>

#include "VoxCore/Math/FMath.h"
#include "VoxCore/Math/Vector.h"

struct FQuat;

struct FRotator {
    FReal Pitch = 0.0;
    FReal Yaw = 0.0;
    FReal Roll = 0.0;

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

    FRotator& operator+=(const FRotator& rhs) noexcept { Pitch += rhs.Pitch; Yaw += rhs.Yaw; Roll += rhs.Roll; return *this; }
    FRotator& operator-=(const FRotator& rhs) noexcept { Pitch -= rhs.Pitch; Yaw -= rhs.Yaw; Roll -= rhs.Roll; return *this; }
    FRotator& operator*=(FReal scalar) noexcept { Pitch *= scalar; Yaw *= scalar; Roll *= scalar; return *this; }

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

    [[nodiscard]] FRotator GetDenormalized() const noexcept {
        return {
            FMath::ClampAxis(Pitch),
            FMath::ClampAxis(Yaw),
            FMath::ClampAxis(Roll)};
    }

    [[nodiscard]] bool IsNearlyZero(FReal tolerance = FMath::KindaSmallNumber) const noexcept {
        return FMath::IsNearlyZero(Pitch, tolerance)
            && FMath::IsNearlyZero(Yaw, tolerance)
            && FMath::IsNearlyZero(Roll, tolerance);
    }

    [[nodiscard]] bool Equals(const FRotator& rhs, FReal tolerance = FMath::KindaSmallNumber) const noexcept {
        return GetNormalized().Euler().Equals(rhs.GetNormalized().Euler(), tolerance);
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

    [[nodiscard]] FQuat Quaternion() const noexcept;
};

inline std::ostream& operator<<(std::ostream& stream, const FRotator& value) {
    stream << "(Pitch=" << value.Pitch << ", Yaw=" << value.Yaw << ", Roll=" << value.Roll << ")";
    return stream;
}
