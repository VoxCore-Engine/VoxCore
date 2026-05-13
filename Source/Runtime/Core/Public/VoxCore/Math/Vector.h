#pragma once

#include <ostream>

#include "VoxCore/Core/Types/Common.h"
#include "VoxCore/Math/FMath.h"

template <typename T>
struct TVector2 {
    T X = T(0);
    T Y = T(0);

    constexpr TVector2() = default;
    constexpr TVector2(T value) : X(value), Y(value) {}
    constexpr TVector2(T x, T y) : X(x), Y(y) {}

    [[nodiscard]] static constexpr TVector2 Zero() noexcept { return {}; }
    [[nodiscard]] static constexpr TVector2 One() noexcept { return {T(1), T(1)}; }
    [[nodiscard]] static constexpr TVector2 UnitX() noexcept { return {T(1), T(0)}; }
    [[nodiscard]] static constexpr TVector2 UnitY() noexcept { return {T(0), T(1)}; }

    [[nodiscard]] constexpr T* GetData() noexcept { return &X; }
    [[nodiscard]] constexpr const T* GetData() const noexcept { return &X; }

    [[nodiscard]] constexpr T& operator[](SIZE_T index) noexcept { return GetData()[index]; }
    [[nodiscard]] constexpr const T& operator[](SIZE_T index) const noexcept { return GetData()[index]; }

    [[nodiscard]] constexpr T SizeSquared() const noexcept { return (X * X) + (Y * Y); }
    [[nodiscard]] inline T Size() const noexcept { return static_cast<T>(FMath::Sqrt(static_cast<FReal>(SizeSquared()))); }
    [[nodiscard]] constexpr bool IsNearlyZero(T tolerance = static_cast<T>(FMath::KindaSmallNumber)) const noexcept {
        return FMath::Abs(X) <= tolerance && FMath::Abs(Y) <= tolerance;
    }
    [[nodiscard]] constexpr bool Equals(const TVector2& rhs, T tolerance = static_cast<T>(FMath::KindaSmallNumber)) const noexcept {
        return FMath::Abs(X - rhs.X) <= tolerance && FMath::Abs(Y - rhs.Y) <= tolerance;
    }

    [[nodiscard]] inline TVector2 GetSafeNormal(T tolerance = static_cast<T>(FMath::SmallNumber)) const noexcept {
        const T size = Size();
        if (size <= tolerance) {
            return Zero();
        }
        return *this / size;
    }

    TVector2& Normalize(T tolerance = static_cast<T>(FMath::SmallNumber)) noexcept {
        *this = GetSafeNormal(tolerance);
        return *this;
    }

    [[nodiscard]] constexpr T GetMax() const noexcept { return FMath::Max(X, Y); }
    [[nodiscard]] constexpr T GetMin() const noexcept { return FMath::Min(X, Y); }
    [[nodiscard]] constexpr TVector2 GetAbs() const noexcept { return {FMath::Abs(X), FMath::Abs(Y)}; }

    [[nodiscard]] static constexpr T Dot(const TVector2& a, const TVector2& b) noexcept {
        return (a.X * b.X) + (a.Y * b.Y);
    }

    [[nodiscard]] static constexpr T Cross(const TVector2& a, const TVector2& b) noexcept {
        return (a.X * b.Y) - (a.Y * b.X);
    }

    [[nodiscard]] static inline T DistSquared(const TVector2& a, const TVector2& b) noexcept {
        return (a - b).SizeSquared();
    }

    [[nodiscard]] static inline T Dist(const TVector2& a, const TVector2& b) noexcept {
        return (a - b).Size();
    }

    [[nodiscard]] constexpr TVector2 operator+(const TVector2& rhs) const noexcept { return {X + rhs.X, Y + rhs.Y}; }
    [[nodiscard]] constexpr TVector2 operator-(const TVector2& rhs) const noexcept { return {X - rhs.X, Y - rhs.Y}; }
    [[nodiscard]] constexpr TVector2 operator-() const noexcept { return {-X, -Y}; }
    [[nodiscard]] constexpr TVector2 operator*(const TVector2& rhs) const noexcept { return {X * rhs.X, Y * rhs.Y}; }
    [[nodiscard]] constexpr TVector2 operator/(const TVector2& rhs) const noexcept { return {X / rhs.X, Y / rhs.Y}; }
    [[nodiscard]] constexpr TVector2 operator*(T scalar) const noexcept { return {X * scalar, Y * scalar}; }
    [[nodiscard]] constexpr TVector2 operator/(T scalar) const noexcept { return {X / scalar, Y / scalar}; }
    [[nodiscard]] constexpr T operator|(const TVector2& rhs) const noexcept { return Dot(*this, rhs); }
    [[nodiscard]] constexpr T operator^(const TVector2& rhs) const noexcept { return Cross(*this, rhs); }

    TVector2& operator+=(const TVector2& rhs) noexcept { X += rhs.X; Y += rhs.Y; return *this; }
    TVector2& operator-=(const TVector2& rhs) noexcept { X -= rhs.X; Y -= rhs.Y; return *this; }
    TVector2& operator*=(const TVector2& rhs) noexcept { X *= rhs.X; Y *= rhs.Y; return *this; }
    TVector2& operator/=(const TVector2& rhs) noexcept { X /= rhs.X; Y /= rhs.Y; return *this; }
    TVector2& operator*=(T scalar) noexcept { X *= scalar; Y *= scalar; return *this; }
    TVector2& operator/=(T scalar) noexcept { X /= scalar; Y /= scalar; return *this; }

    [[nodiscard]] constexpr bool operator==(const TVector2& rhs) const noexcept { return X == rhs.X && Y == rhs.Y; }
    [[nodiscard]] constexpr bool operator!=(const TVector2& rhs) const noexcept { return !(*this == rhs); }
};

template <typename T>
struct TVector3 {
    T X = T(0);
    T Y = T(0);
    T Z = T(0);

    constexpr TVector3() = default;
    constexpr TVector3(T value) : X(value), Y(value), Z(value) {}
    constexpr TVector3(T x, T y, T z) : X(x), Y(y), Z(z) {}
    constexpr TVector3(const TVector2<T>& xy, T z) : X(xy.X), Y(xy.Y), Z(z) {}

    [[nodiscard]] static constexpr TVector3 Zero() noexcept { return {}; }
    [[nodiscard]] static constexpr TVector3 One() noexcept { return {T(1), T(1), T(1)}; }
    [[nodiscard]] static constexpr TVector3 ForwardVector() noexcept { return {T(1), T(0), T(0)}; }
    [[nodiscard]] static constexpr TVector3 BackwardVector() noexcept { return {T(-1), T(0), T(0)}; }
    [[nodiscard]] static constexpr TVector3 RightVector() noexcept { return {T(0), T(1), T(0)}; }
    [[nodiscard]] static constexpr TVector3 LeftVector() noexcept { return {T(0), T(-1), T(0)}; }
    [[nodiscard]] static constexpr TVector3 UpVector() noexcept { return {T(0), T(0), T(1)}; }
    [[nodiscard]] static constexpr TVector3 DownVector() noexcept { return {T(0), T(0), T(-1)}; }

    [[nodiscard]] constexpr T* GetData() noexcept { return &X; }
    [[nodiscard]] constexpr const T* GetData() const noexcept { return &X; }

    [[nodiscard]] constexpr T& operator[](SIZE_T index) noexcept { return GetData()[index]; }
    [[nodiscard]] constexpr const T& operator[](SIZE_T index) const noexcept { return GetData()[index]; }

    [[nodiscard]] constexpr T SizeSquared() const noexcept { return (X * X) + (Y * Y) + (Z * Z); }
    [[nodiscard]] inline T Size() const noexcept { return static_cast<T>(FMath::Sqrt(static_cast<FReal>(SizeSquared()))); }
    [[nodiscard]] constexpr T SizeSquared2D() const noexcept { return (X * X) + (Y * Y); }
    [[nodiscard]] inline T Size2D() const noexcept { return static_cast<T>(FMath::Sqrt(static_cast<FReal>(SizeSquared2D()))); }
    [[nodiscard]] constexpr bool IsNearlyZero(T tolerance = static_cast<T>(FMath::KindaSmallNumber)) const noexcept {
        return FMath::Abs(X) <= tolerance && FMath::Abs(Y) <= tolerance && FMath::Abs(Z) <= tolerance;
    }
    [[nodiscard]] constexpr bool Equals(const TVector3& rhs, T tolerance = static_cast<T>(FMath::KindaSmallNumber)) const noexcept {
        return FMath::Abs(X - rhs.X) <= tolerance
            && FMath::Abs(Y - rhs.Y) <= tolerance
            && FMath::Abs(Z - rhs.Z) <= tolerance;
    }

    [[nodiscard]] inline TVector3 GetSafeNormal(T tolerance = static_cast<T>(FMath::SmallNumber)) const noexcept {
        const T size = Size();
        if (size <= tolerance) {
            return Zero();
        }
        return *this / size;
    }

    [[nodiscard]] inline TVector3 GetSafeNormal2D(T tolerance = static_cast<T>(FMath::SmallNumber)) const noexcept {
        const T size = Size2D();
        if (size <= tolerance) {
            return Zero();
        }
        return {X / size, Y / size, T(0)};
    }

    TVector3& Normalize(T tolerance = static_cast<T>(FMath::SmallNumber)) noexcept {
        *this = GetSafeNormal(tolerance);
        return *this;
    }

    [[nodiscard]] constexpr T GetMax() const noexcept { return FMath::Max3(X, Y, Z); }
    [[nodiscard]] constexpr T GetMin() const noexcept { return FMath::Min3(X, Y, Z); }
    [[nodiscard]] constexpr TVector3 GetAbs() const noexcept { return {FMath::Abs(X), FMath::Abs(Y), FMath::Abs(Z)}; }
    [[nodiscard]] constexpr TVector3 GetSignVector() const noexcept {
        return {FMath::Sign(X), FMath::Sign(Y), FMath::Sign(Z)};
    }

    [[nodiscard]] static constexpr T Dot(const TVector3& a, const TVector3& b) noexcept {
        return (a.X * b.X) + (a.Y * b.Y) + (a.Z * b.Z);
    }

    [[nodiscard]] static constexpr TVector3 Cross(const TVector3& a, const TVector3& b) noexcept {
        return {
            (a.Y * b.Z) - (a.Z * b.Y),
            (a.Z * b.X) - (a.X * b.Z),
            (a.X * b.Y) - (a.Y * b.X)};
    }

    [[nodiscard]] static inline T DistSquared(const TVector3& a, const TVector3& b) noexcept {
        return (a - b).SizeSquared();
    }

    [[nodiscard]] static inline T Dist(const TVector3& a, const TVector3& b) noexcept {
        return (a - b).Size();
    }

    [[nodiscard]] static inline T DistSquared2D(const TVector3& a, const TVector3& b) noexcept {
        return (a - b).SizeSquared2D();
    }

    [[nodiscard]] static inline T Dist2D(const TVector3& a, const TVector3& b) noexcept {
        return (a - b).Size2D();
    }

    [[nodiscard]] inline TVector3 ProjectOnTo(const TVector3& other) const noexcept {
        const T denominator = Dot(other, other);
        if (FMath::IsNearlyZero(static_cast<FReal>(denominator), FMath::SmallNumber)) {
            return Zero();
        }
        return other * (Dot(*this, other) / denominator);
    }

    [[nodiscard]] constexpr TVector3 operator+(const TVector3& rhs) const noexcept { return {X + rhs.X, Y + rhs.Y, Z + rhs.Z}; }
    [[nodiscard]] constexpr TVector3 operator-(const TVector3& rhs) const noexcept { return {X - rhs.X, Y - rhs.Y, Z - rhs.Z}; }
    [[nodiscard]] constexpr TVector3 operator-() const noexcept { return {-X, -Y, -Z}; }
    [[nodiscard]] constexpr TVector3 operator*(const TVector3& rhs) const noexcept { return {X * rhs.X, Y * rhs.Y, Z * rhs.Z}; }
    [[nodiscard]] constexpr TVector3 operator/(const TVector3& rhs) const noexcept { return {X / rhs.X, Y / rhs.Y, Z / rhs.Z}; }
    [[nodiscard]] constexpr TVector3 operator*(T scalar) const noexcept { return {X * scalar, Y * scalar, Z * scalar}; }
    [[nodiscard]] constexpr TVector3 operator/(T scalar) const noexcept { return {X / scalar, Y / scalar, Z / scalar}; }
    [[nodiscard]] constexpr T operator|(const TVector3& rhs) const noexcept { return Dot(*this, rhs); }
    [[nodiscard]] constexpr TVector3 operator^(const TVector3& rhs) const noexcept { return Cross(*this, rhs); }

    TVector3& operator+=(const TVector3& rhs) noexcept { X += rhs.X; Y += rhs.Y; Z += rhs.Z; return *this; }
    TVector3& operator-=(const TVector3& rhs) noexcept { X -= rhs.X; Y -= rhs.Y; Z -= rhs.Z; return *this; }
    TVector3& operator*=(const TVector3& rhs) noexcept { X *= rhs.X; Y *= rhs.Y; Z *= rhs.Z; return *this; }
    TVector3& operator/=(const TVector3& rhs) noexcept { X /= rhs.X; Y /= rhs.Y; Z /= rhs.Z; return *this; }
    TVector3& operator*=(T scalar) noexcept { X *= scalar; Y *= scalar; Z *= scalar; return *this; }
    TVector3& operator/=(T scalar) noexcept { X /= scalar; Y /= scalar; Z /= scalar; return *this; }

    [[nodiscard]] constexpr bool operator==(const TVector3& rhs) const noexcept {
        return X == rhs.X && Y == rhs.Y && Z == rhs.Z;
    }

    [[nodiscard]] constexpr bool operator!=(const TVector3& rhs) const noexcept { return !(*this == rhs); }
};

template <typename T>
struct TVector4 {
    T X = T(0);
    T Y = T(0);
    T Z = T(0);
    T W = T(0);

    constexpr TVector4() = default;
    constexpr TVector4(T value) : X(value), Y(value), Z(value), W(value) {}
    constexpr TVector4(T x, T y, T z, T w) : X(x), Y(y), Z(z), W(w) {}
    constexpr TVector4(const TVector3<T>& xyz, T w) : X(xyz.X), Y(xyz.Y), Z(xyz.Z), W(w) {}

    [[nodiscard]] static constexpr TVector4 Zero() noexcept { return {}; }
    [[nodiscard]] static constexpr TVector4 One() noexcept { return {T(1), T(1), T(1), T(1)}; }
    [[nodiscard]] constexpr T* GetData() noexcept { return &X; }
    [[nodiscard]] constexpr const T* GetData() const noexcept { return &X; }

    [[nodiscard]] constexpr T& operator[](SIZE_T index) noexcept { return GetData()[index]; }
    [[nodiscard]] constexpr const T& operator[](SIZE_T index) const noexcept { return GetData()[index]; }

    [[nodiscard]] constexpr T SizeSquared() const noexcept { return (X * X) + (Y * Y) + (Z * Z) + (W * W); }
    [[nodiscard]] inline T Size() const noexcept { return static_cast<T>(FMath::Sqrt(static_cast<FReal>(SizeSquared()))); }
    [[nodiscard]] constexpr bool Equals(const TVector4& rhs, T tolerance = static_cast<T>(FMath::KindaSmallNumber)) const noexcept {
        return FMath::Abs(X - rhs.X) <= tolerance
            && FMath::Abs(Y - rhs.Y) <= tolerance
            && FMath::Abs(Z - rhs.Z) <= tolerance
            && FMath::Abs(W - rhs.W) <= tolerance;
    }

    [[nodiscard]] static constexpr T Dot(const TVector4& a, const TVector4& b) noexcept {
        return (a.X * b.X) + (a.Y * b.Y) + (a.Z * b.Z) + (a.W * b.W);
    }

    [[nodiscard]] constexpr TVector4 operator+(const TVector4& rhs) const noexcept {
        return {X + rhs.X, Y + rhs.Y, Z + rhs.Z, W + rhs.W};
    }

    [[nodiscard]] constexpr TVector4 operator-(const TVector4& rhs) const noexcept {
        return {X - rhs.X, Y - rhs.Y, Z - rhs.Z, W - rhs.W};
    }

    [[nodiscard]] constexpr TVector4 operator-() const noexcept {
        return {-X, -Y, -Z, -W};
    }

    [[nodiscard]] constexpr TVector4 operator*(const TVector4& rhs) const noexcept {
        return {X * rhs.X, Y * rhs.Y, Z * rhs.Z, W * rhs.W};
    }

    [[nodiscard]] constexpr TVector4 operator/(const TVector4& rhs) const noexcept {
        return {X / rhs.X, Y / rhs.Y, Z / rhs.Z, W / rhs.W};
    }

    [[nodiscard]] constexpr TVector4 operator*(T scalar) const noexcept {
        return {X * scalar, Y * scalar, Z * scalar, W * scalar};
    }

    [[nodiscard]] constexpr TVector4 operator/(T scalar) const noexcept {
        return {X / scalar, Y / scalar, Z / scalar, W / scalar};
    }

    TVector4& operator+=(const TVector4& rhs) noexcept { X += rhs.X; Y += rhs.Y; Z += rhs.Z; W += rhs.W; return *this; }
    TVector4& operator-=(const TVector4& rhs) noexcept { X -= rhs.X; Y -= rhs.Y; Z -= rhs.Z; W -= rhs.W; return *this; }
    TVector4& operator*=(const TVector4& rhs) noexcept { X *= rhs.X; Y *= rhs.Y; Z *= rhs.Z; W *= rhs.W; return *this; }
    TVector4& operator/=(const TVector4& rhs) noexcept { X /= rhs.X; Y /= rhs.Y; Z /= rhs.Z; W /= rhs.W; return *this; }
    TVector4& operator*=(T scalar) noexcept { X *= scalar; Y *= scalar; Z *= scalar; W *= scalar; return *this; }
    TVector4& operator/=(T scalar) noexcept { X /= scalar; Y /= scalar; Z /= scalar; W /= scalar; return *this; }

    [[nodiscard]] constexpr bool operator==(const TVector4& rhs) const noexcept {
        return X == rhs.X && Y == rhs.Y && Z == rhs.Z && W == rhs.W;
    }

    [[nodiscard]] constexpr bool operator!=(const TVector4& rhs) const noexcept { return !(*this == rhs); }
};

template <typename T>
[[nodiscard]] constexpr TVector2<T> operator*(T scalar, const TVector2<T>& vector) noexcept {
    return vector * scalar;
}

template <typename T>
[[nodiscard]] constexpr TVector3<T> operator*(T scalar, const TVector3<T>& vector) noexcept {
    return vector * scalar;
}

template <typename T>
[[nodiscard]] constexpr TVector4<T> operator*(T scalar, const TVector4<T>& vector) noexcept {
    return vector * scalar;
}

template <typename T>
inline std::ostream& operator<<(std::ostream& stream, const TVector2<T>& value) {
    stream << "(" << value.X << ", " << value.Y << ")";
    return stream;
}

template <typename T>
inline std::ostream& operator<<(std::ostream& stream, const TVector3<T>& value) {
    stream << "(" << value.X << ", " << value.Y << ", " << value.Z << ")";
    return stream;
}

template <typename T>
inline std::ostream& operator<<(std::ostream& stream, const TVector4<T>& value) {
    stream << "(" << value.X << ", " << value.Y << ", " << value.Z << ", " << value.W << ")";
    return stream;
}

using FVector2D = TVector2<FReal>;
using FVector2f = TVector2<float32>;
using FVector = TVector3<FReal>;
using FVector3f = TVector3<float32>;
using FVector4 = TVector4<FReal>;
using FVector4f = TVector4<float32>;
using FIntPoint = TVector2<int32>;
using FIntVector = TVector3<int32>;
