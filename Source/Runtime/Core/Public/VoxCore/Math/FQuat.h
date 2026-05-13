#pragma once

#include <ostream>

#include "VoxCore/Math/FRotator.h"

struct FQuat {
    FReal X = 0.0;
    FReal Y = 0.0;
    FReal Z = 0.0;
    FReal W = 1.0;

    constexpr FQuat() = default;
    constexpr FQuat(FReal x, FReal y, FReal z, FReal w) : X(x), Y(y), Z(z), W(w) {}

    [[nodiscard]] static constexpr FQuat Identity() noexcept { return {}; }

    [[nodiscard]] static FQuat MakeFromAxisAngle(const FVector& axis, FReal angleRadians) noexcept {
        const FVector normalizedAxis = axis.GetSafeNormal();
        const FReal halfAngle = angleRadians * 0.5;
        const FReal sinHalf = FMath::Sin(halfAngle);
        const FReal cosHalf = FMath::Cos(halfAngle);
        return {
            normalizedAxis.X * sinHalf,
            normalizedAxis.Y * sinHalf,
            normalizedAxis.Z * sinHalf,
            cosHalf};
    }

    [[nodiscard]] static FQuat MakeFromRotator(const FRotator& rotator) noexcept {
        const FQuat yaw = MakeFromAxisAngle(FVector::UpVector(), FMath::DegreesToRadiansFn(rotator.Yaw));
        const FQuat pitch = MakeFromAxisAngle(FVector::RightVector(), FMath::DegreesToRadiansFn(rotator.Pitch));
        const FQuat roll = MakeFromAxisAngle(FVector::ForwardVector(), FMath::DegreesToRadiansFn(rotator.Roll));
        return (yaw * pitch * roll).GetNormalized();
    }

    [[nodiscard]] static FQuat MakeFromEuler(const FVector& eulerDegrees) noexcept {
        return MakeFromRotator(FRotator(eulerDegrees.X, eulerDegrees.Y, eulerDegrees.Z));
    }

    [[nodiscard]] FReal SizeSquared() const noexcept { return (X * X) + (Y * Y) + (Z * Z) + (W * W); }
    [[nodiscard]] FReal Size() const noexcept { return FMath::Sqrt(SizeSquared()); }

    FQuat& Normalize() noexcept {
        const FReal size = Size();
        if (size > FMath::SmallNumber) {
            const FReal invSize = 1.0 / size;
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

    [[nodiscard]] bool IsNormalized(FReal tolerance = FMath::KindaSmallNumber) const noexcept {
        return FMath::IsNearlyEqual(SizeSquared(), 1.0, tolerance);
    }

    [[nodiscard]] bool Equals(const FQuat& rhs, FReal tolerance = FMath::KindaSmallNumber) const noexcept {
        return FMath::IsNearlyEqual(X, rhs.X, tolerance)
            && FMath::IsNearlyEqual(Y, rhs.Y, tolerance)
            && FMath::IsNearlyEqual(Z, rhs.Z, tolerance)
            && FMath::IsNearlyEqual(W, rhs.W, tolerance);
    }

    [[nodiscard]] static constexpr FReal Dot(const FQuat& lhs, const FQuat& rhs) noexcept {
        return (lhs.X * rhs.X) + (lhs.Y * rhs.Y) + (lhs.Z * rhs.Z) + (lhs.W * rhs.W);
    }

    [[nodiscard]] FQuat Inverse() const noexcept {
        const FReal norm = SizeSquared();
        if (norm <= FMath::SmallNumber) {
            return Identity();
        }

        const FReal invNorm = 1.0 / norm;
        return {-X * invNorm, -Y * invNorm, -Z * invNorm, W * invNorm};
    }

    [[nodiscard]] FVector RotateVector(const FVector& value) const noexcept {
        const FQuat normalized = GetNormalized();
        const FVector axis(normalized.X, normalized.Y, normalized.Z);
        const FVector uv = FVector::Cross(axis, value);
        const FVector uuv = FVector::Cross(axis, uv);
        return value + ((uv * normalized.W) + uuv) * 2.0;
    }

    [[nodiscard]] FVector UnrotateVector(const FVector& value) const noexcept {
        return Inverse().RotateVector(value);
    }

    [[nodiscard]] FVector GetForwardVector() const noexcept { return RotateVector(FVector::ForwardVector()); }
    [[nodiscard]] FVector GetRightVector() const noexcept { return RotateVector(FVector::RightVector()); }
    [[nodiscard]] FVector GetUpVector() const noexcept { return RotateVector(FVector::UpVector()); }
    [[nodiscard]] FVector GetAxisX() const noexcept { return GetForwardVector(); }
    [[nodiscard]] FVector GetAxisY() const noexcept { return GetRightVector(); }
    [[nodiscard]] FVector GetAxisZ() const noexcept { return GetUpVector(); }

    [[nodiscard]] FRotator Rotator() const noexcept {
        const FQuat normalized = GetNormalized();

        const FReal sinRollCosPitch = 2.0 * ((normalized.W * normalized.X) + (normalized.Y * normalized.Z));
        const FReal cosRollCosPitch = 1.0 - 2.0 * ((normalized.X * normalized.X) + (normalized.Y * normalized.Y));
        const FReal roll = FMath::Atan2(sinRollCosPitch, cosRollCosPitch);

        const FReal sinPitch = 2.0 * ((normalized.W * normalized.Y) - (normalized.Z * normalized.X));
        const FReal pitch = FMath::Abs(sinPitch) >= 1.0
            ? std::copysign(FMath::HalfPi, sinPitch)
            : FMath::Asin(sinPitch);

        const FReal sinYawCosPitch = 2.0 * ((normalized.W * normalized.Z) + (normalized.X * normalized.Y));
        const FReal cosYawCosPitch = 1.0 - 2.0 * ((normalized.Y * normalized.Y) + (normalized.Z * normalized.Z));
        const FReal yaw = FMath::Atan2(sinYawCosPitch, cosYawCosPitch);

        return FRotator(
            FMath::RadiansToDegreesFn(pitch),
            FMath::RadiansToDegreesFn(yaw),
            FMath::RadiansToDegreesFn(roll));
    }

    [[nodiscard]] FQuat operator+(const FQuat& rhs) const noexcept {
        return {X + rhs.X, Y + rhs.Y, Z + rhs.Z, W + rhs.W};
    }

    [[nodiscard]] FQuat operator-(const FQuat& rhs) const noexcept {
        return {X - rhs.X, Y - rhs.Y, Z - rhs.Z, W - rhs.W};
    }

    [[nodiscard]] FQuat operator*(FReal scalar) const noexcept {
        return {X * scalar, Y * scalar, Z * scalar, W * scalar};
    }

    [[nodiscard]] FQuat operator*(const FQuat& rhs) const noexcept {
        return {
            (W * rhs.X) + (X * rhs.W) + (Y * rhs.Z) - (Z * rhs.Y),
            (W * rhs.Y) - (X * rhs.Z) + (Y * rhs.W) + (Z * rhs.X),
            (W * rhs.Z) + (X * rhs.Y) - (Y * rhs.X) + (Z * rhs.W),
            (W * rhs.W) - (X * rhs.X) - (Y * rhs.Y) - (Z * rhs.Z)};
    }

    FQuat& operator+=(const FQuat& rhs) noexcept { X += rhs.X; Y += rhs.Y; Z += rhs.Z; W += rhs.W; return *this; }
    FQuat& operator-=(const FQuat& rhs) noexcept { X -= rhs.X; Y -= rhs.Y; Z -= rhs.Z; W -= rhs.W; return *this; }
    FQuat& operator*=(const FQuat& rhs) noexcept { *this = *this * rhs; return *this; }
    FQuat& operator*=(FReal scalar) noexcept { X *= scalar; Y *= scalar; Z *= scalar; W *= scalar; return *this; }

    [[nodiscard]] constexpr bool operator==(const FQuat& rhs) const noexcept {
        return X == rhs.X && Y == rhs.Y && Z == rhs.Z && W == rhs.W;
    }

    [[nodiscard]] constexpr bool operator!=(const FQuat& rhs) const noexcept { return !(*this == rhs); }
};

inline FQuat FRotator::Quaternion() const noexcept {
    return FQuat::MakeFromRotator(*this);
}

inline std::ostream& operator<<(std::ostream& stream, const FQuat& value) {
    stream << "(X=" << value.X << ", Y=" << value.Y << ", Z=" << value.Z << ", W=" << value.W << ")";
    return stream;
}
