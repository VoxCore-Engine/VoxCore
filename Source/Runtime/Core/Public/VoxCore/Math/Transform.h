#pragma once

#include <ostream>

#include "VoxCore/Math/Matrix.h"

struct FTransform {
    FQuat Rotation = FQuat::Identity();
    FVector Translation = FVector::Zero();
    FVector Scale3D = FVector::One();

    constexpr FTransform() = default;
    constexpr explicit FTransform(const FVector& translation)
        : Translation(translation) {}
    constexpr FTransform(const FQuat& rotation, const FVector& translation, const FVector& scale3D = FVector::One())
        : Rotation(rotation), Translation(translation), Scale3D(scale3D) {}
    constexpr FTransform(const FRotator& rotation, const FVector& translation, const FVector& scale3D = FVector::One())
        : Rotation(rotation.Quaternion()), Translation(translation), Scale3D(scale3D) {}

    [[nodiscard]] static constexpr FTransform Identity() noexcept { return {}; }

    FTransform& NormalizeRotation() noexcept {
        Rotation.Normalize();
        return *this;
    }

    [[nodiscard]] bool IsNearlyIdentity(FReal tolerance = FMath::KindaSmallNumber) const noexcept {
        return Rotation.Equals(FQuat::Identity(), tolerance)
            && Translation.IsNearlyZero(tolerance)
            && Scale3D.Equals(FVector::One(), tolerance);
    }

    [[nodiscard]] FVector TransformPosition(const FVector& value) const noexcept {
        return Rotation.RotateVector(value * Scale3D) + Translation;
    }

    [[nodiscard]] FVector TransformVector(const FVector& value) const noexcept {
        return Rotation.RotateVector(value * Scale3D);
    }

    [[nodiscard]] FVector TransformVectorNoScale(const FVector& value) const noexcept {
        return Rotation.RotateVector(value);
    }

    [[nodiscard]] FVector InverseTransformPosition(const FVector& value) const noexcept {
        const FVector unrotated = Rotation.Inverse().RotateVector(value - Translation);
        return {
            FMath::IsNearlyZero(Scale3D.X) ? 0.0 : unrotated.X / Scale3D.X,
            FMath::IsNearlyZero(Scale3D.Y) ? 0.0 : unrotated.Y / Scale3D.Y,
            FMath::IsNearlyZero(Scale3D.Z) ? 0.0 : unrotated.Z / Scale3D.Z};
    }

    [[nodiscard]] FVector InverseTransformVector(const FVector& value) const noexcept {
        const FVector unrotated = Rotation.Inverse().RotateVector(value);
        return {
            FMath::IsNearlyZero(Scale3D.X) ? 0.0 : unrotated.X / Scale3D.X,
            FMath::IsNearlyZero(Scale3D.Y) ? 0.0 : unrotated.Y / Scale3D.Y,
            FMath::IsNearlyZero(Scale3D.Z) ? 0.0 : unrotated.Z / Scale3D.Z};
    }

    [[nodiscard]] FMatrix ToMatrixWithScale() const noexcept {
        FMatrix matrix = FMatrix::Identity();
        const FVector axisX = Rotation.GetAxisX() * Scale3D.X;
        const FVector axisY = Rotation.GetAxisY() * Scale3D.Y;
        const FVector axisZ = Rotation.GetAxisZ() * Scale3D.Z;

        matrix(0, 0) = axisX.X;
        matrix(1, 0) = axisX.Y;
        matrix(2, 0) = axisX.Z;

        matrix(0, 1) = axisY.X;
        matrix(1, 1) = axisY.Y;
        matrix(2, 1) = axisY.Z;

        matrix(0, 2) = axisZ.X;
        matrix(1, 2) = axisZ.Y;
        matrix(2, 2) = axisZ.Z;

        matrix(0, 3) = Translation.X;
        matrix(1, 3) = Translation.Y;
        matrix(2, 3) = Translation.Z;
        return matrix;
    }

    [[nodiscard]] FMatrix ToMatrixNoScale() const noexcept {
        return FTransform(Rotation, Translation, FVector::One()).ToMatrixWithScale();
    }

    [[nodiscard]] FTransform Inverse() const noexcept {
        const FQuat InverseRotation = Rotation.Inverse();
        const FVector SafeInverseScale(
            FMath::IsNearlyZero(Scale3D.X) ? 0.0 : 1.0 / Scale3D.X,
            FMath::IsNearlyZero(Scale3D.Y) ? 0.0 : 1.0 / Scale3D.Y,
            FMath::IsNearlyZero(Scale3D.Z) ? 0.0 : 1.0 / Scale3D.Z);
        const FVector InverseTranslation = InverseRotation.RotateVector((-Translation) * SafeInverseScale);
        return FTransform(InverseRotation, InverseTranslation, SafeInverseScale);
    }

    [[nodiscard]] FTransform operator*(const FTransform& rhs) const noexcept {
        return {
            (Rotation * rhs.Rotation).GetNormalized(),
            TransformPosition(rhs.Translation),
            Scale3D * rhs.Scale3D};
    }

    FTransform& operator*=(const FTransform& rhs) noexcept {
        *this = *this * rhs;
        return *this;
    }

    [[nodiscard]] bool Equals(const FTransform& rhs, FReal tolerance = FMath::KindaSmallNumber) const noexcept {
        return Rotation.Equals(rhs.Rotation, tolerance)
            && Translation.Equals(rhs.Translation, tolerance)
            && Scale3D.Equals(rhs.Scale3D, tolerance);
    }
};

inline std::ostream& operator<<(std::ostream& stream, const FTransform& value) {
    stream
        << "(Translation=" << value.Translation
        << ", Rotation=" << value.Rotation
        << ", Scale3D=" << value.Scale3D
        << ")";
    return stream;
}
