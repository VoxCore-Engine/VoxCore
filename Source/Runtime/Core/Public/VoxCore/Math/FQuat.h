//
// Created by IDKTHIS on 12.05.2026.
//

#pragma once
#include "FRotator.h"

#include "FMath.h"


struct FQuat {
    FReal X = 0.0f;
    FReal Y = 0.0f;
    FReal Z = 0.0f;
    FReal W = 1.0f;

    constexpr FQuat() = default;
    constexpr FQuat(FReal x, FReal y, FReal z, FReal w) : X(x), Y(y), Z(z), W(w) {}

    [[nodiscard]] static constexpr FQuat Identity() noexcept { return {}; }

    [[nodiscard]] static FQuat MakeFromAxisAngle(const FVector& axis, FReal angleRadians) noexcept {
        const FVector normalizedAxis = axis.GetSafeNormal();
        const FReal halfAngle = angleRadians * 0.5f;
        const FReal sinHalf = FMath::Sin(halfAngle);
        const FReal cosHalf = FMath::Cos(halfAngle);
        return {
            normalizedAxis.X * sinHalf,
            normalizedAxis.Y * sinHalf,
            normalizedAxis.Z * sinHalf,
            cosHalf};
    }

    [[nodiscard]] static FQuat MakeFromRotator(const FRotator& rotator) noexcept {
        const FReal pitch = FMath::DegreesToRadiansFn(rotator.Pitch) * 0.5f;
        const FReal yaw = FMath::DegreesToRadiansFn(rotator.Yaw) * 0.5f;
        const FReal roll = FMath::DegreesToRadiansFn(rotator.Roll) * 0.5f;

        const FReal sp = FMath::Sin(pitch);
        const FReal cp = FMath::Cos(pitch);
        const FReal sy = FMath::Sin(yaw);
        const FReal cy = FMath::Cos(yaw);
        const FReal sr = FMath::Sin(roll);
        const FReal cr = FMath::Cos(roll);

        return {
            (cr * sp * sy) - (sr * cp * cy),
            (-cr * sp * cy) - (sr * cp * sy),
            (cr * cp * sy) - (sr * sp * cy),
            (cr * cp * cy) + (sr * sp * sy)};
    }

    [[nodiscard]] static FQuat MakeFromEuler(const FVector& eulerDegrees) noexcept {
        return MakeFromRotator(FRotator(eulerDegrees.X, eulerDegrees.Y, eulerDegrees.Z));
    }

    [[nodiscard]] FReal SizeSquared() const noexcept { return (X * X) + (Y * Y) + (Z * Z) + (W * W); }
    [[nodiscard]] FReal Size() const noexcept { return FMath::Sqrt(SizeSquared()); }

    FQuat& Normalize() noexcept {
        const FReal size = Size();
        if (size > FMath::SmallNumber) {
            const FReal invSize = 1.0f / size;
            X *= invSize;
            Y *= invSize;
            Z *= invSize;
            W *= invSize;
        } else {
            *this = Identity();
        }
        return *this;
    }

    [[nodiscard]] FQuat GetNormalized() const noexcept {
        FQuat copy(*this);
        return copy.Normalize();
    }

    [[nodiscard]] FQuat Inverse() const noexcept {
        const FReal norm = SizeSquared();
        if (norm <= FMath::SmallNumber) {
            return Identity();
        }
        const FReal invNorm = 1.0f / norm;
        return {-X * invNorm, -Y * invNorm, -Z * invNorm, W * invNorm};
    }

    [[nodiscard]] FVector RotateVector(const FVector& value) const noexcept {
        const FQuat vectorQuat(value.X, value.Y, value.Z, 0.0f);
        const FQuat result = (*this) * vectorQuat * Inverse();
        return {result.X, result.Y, result.Z};
    }

    [[nodiscard]] FRotator Rotator() const noexcept {
        const FQuat normalized = GetNormalized();

        const FReal sinRCosP = 2.0f * ((normalized.W * normalized.X) + (normalized.Y * normalized.Z));
        const FReal cosRCosP = 1.0f - 2.0f * ((normalized.X * normalized.X) + (normalized.Y * normalized.Y));
        const FReal roll = FMath::Atan2(sinRCosP, cosRCosP);

        const FReal sinP = 2.0f * ((normalized.W * normalized.Y) - (normalized.Z * normalized.X));
        FReal pitch = 0.0f;
        if (FMath::Abs(sinP) >= 1.0f) {
            pitch = std::copysign(FMath::HalfPi, sinP);
        } else {
            pitch = FMath::Asin(sinP);
        }

        const FReal sinYCosP = 2.0f * ((normalized.W * normalized.Z) + (normalized.X * normalized.Y));
        const FReal cosYCosP = 1.0f - 2.0f * ((normalized.Y * normalized.Y) + (normalized.Z * normalized.Z));
        const FReal yaw = FMath::Atan2(sinYCosP, cosYCosP);

        return FRotator(
            FMath::RadiansToDegreesFn(pitch),
            FMath::RadiansToDegreesFn(yaw),
            FMath::RadiansToDegreesFn(roll));
    }

    [[nodiscard]] FQuat operator*(const FQuat& rhs) const noexcept {
        return {
            (W * rhs.X) + (X * rhs.W) + (Y * rhs.Z) - (Z * rhs.Y),
            (W * rhs.Y) - (X * rhs.Z) + (Y * rhs.W) + (Z * rhs.X),
            (W * rhs.Z) + (X * rhs.Y) - (Y * rhs.X) + (Z * rhs.W),
            (W * rhs.W) - (X * rhs.X) - (Y * rhs.Y) - (Z * rhs.Z)};
    }
};

inline std::ostream& operator<<(std::ostream& stream, const FQuat& value) {
    stream << "(X=" << value.X << ", Y=" << value.Y << ", Z=" << value.Z << ", W=" << value.W << ")";
    return stream;
}
