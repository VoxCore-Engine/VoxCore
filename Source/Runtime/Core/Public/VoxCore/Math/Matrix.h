#pragma once

#include <array>
#include <initializer_list>
#include <ostream>
#include <type_traits>
#include <utility>

#include "VoxCore/Core/Types/Common.h"
#include "VoxCore/Math/FQuat.h"
#include "VoxCore/Math/Vector.h"

template <typename T, SIZE_T Rows, SIZE_T Cols>
class TMatrix {
public:
    static_assert(Rows > 0 && Cols > 0);

    using ValueType = T;
    static constexpr SIZE_T RowCount = Rows;
    static constexpr SIZE_T ColumnCount = Cols;
    static constexpr SIZE_T ElementCount = Rows * Cols;
    static constexpr bool IsSquare = Rows == Cols;

    struct FRowProxy {
        T* RowData = nullptr;

        [[nodiscard]] T& operator[](SIZE_T column) noexcept { return RowData[column]; }
        [[nodiscard]] const T& operator[](SIZE_T column) const noexcept { return RowData[column]; }
    };

    struct FConstRowProxy {
        const T* RowData = nullptr;

        [[nodiscard]] const T& operator[](SIZE_T column) const noexcept { return RowData[column]; }
    };

    constexpr TMatrix() = default;
    explicit constexpr TMatrix(const std::array<T, ElementCount>& values) : Data(values) {}

    constexpr TMatrix(std::initializer_list<std::initializer_list<T>> rows) {
        SIZE_T rowIndex = 0;
        for (const auto& row : rows) {
            SIZE_T columnIndex = 0;
            for (const T& value : row) {
                if (rowIndex < Rows && columnIndex < Cols) {
                    (*this)(rowIndex, columnIndex) = value;
                }
                ++columnIndex;
            }
            ++rowIndex;
        }
    }

    [[nodiscard]] static constexpr TMatrix Zero() noexcept { return {}; }

    [[nodiscard]] static constexpr TMatrix Identity() noexcept requires (IsSquare) {
        TMatrix matrix;
        for (SIZE_T index = 0; index < Rows; ++index) {
            matrix(index, index) = T(1);
        }
        return matrix;
    }

    [[nodiscard]] static constexpr TMatrix Diagonal(T value) noexcept requires (IsSquare) {
        TMatrix matrix;
        for (SIZE_T index = 0; index < Rows; ++index) {
            matrix(index, index) = value;
        }
        return matrix;
    }

    [[nodiscard]] static constexpr TMatrix MakeTranslation(const TVector3<T>& translation) noexcept
        requires (Rows == 4 && Cols == 4) {
        TMatrix matrix = Identity();
        matrix(0, 3) = translation.X;
        matrix(1, 3) = translation.Y;
        matrix(2, 3) = translation.Z;
        return matrix;
    }

    [[nodiscard]] static constexpr TMatrix MakeScale(const TVector3<T>& scale) noexcept
        requires (Rows == 4 && Cols == 4) {
        TMatrix matrix = Identity();
        matrix(0, 0) = scale.X;
        matrix(1, 1) = scale.Y;
        matrix(2, 2) = scale.Z;
        return matrix;
    }

    [[nodiscard]] static TMatrix MakeRotation(const FQuat& rotation) noexcept
        requires (Rows == 4 && Cols == 4 && std::is_floating_point_v<T>) {
        const FQuat normalized = rotation.GetNormalized();

        const T xx = static_cast<T>(normalized.X * normalized.X);
        const T yy = static_cast<T>(normalized.Y * normalized.Y);
        const T zz = static_cast<T>(normalized.Z * normalized.Z);
        const T xy = static_cast<T>(normalized.X * normalized.Y);
        const T xz = static_cast<T>(normalized.X * normalized.Z);
        const T yz = static_cast<T>(normalized.Y * normalized.Z);
        const T wx = static_cast<T>(normalized.W * normalized.X);
        const T wy = static_cast<T>(normalized.W * normalized.Y);
        const T wz = static_cast<T>(normalized.W * normalized.Z);

        TMatrix matrix = Identity();
        matrix(0, 0) = T(1) - T(2) * (yy + zz);
        matrix(0, 1) = T(2) * (xy - wz);
        matrix(0, 2) = T(2) * (xz + wy);
        matrix(1, 0) = T(2) * (xy + wz);
        matrix(1, 1) = T(1) - T(2) * (xx + zz);
        matrix(1, 2) = T(2) * (yz - wx);
        matrix(2, 0) = T(2) * (xz - wy);
        matrix(2, 1) = T(2) * (yz + wx);
        matrix(2, 2) = T(1) - T(2) * (xx + yy);
        return matrix;
    }

    [[nodiscard]] static TMatrix MakeRotation(const FRotator& rotation) noexcept
        requires (Rows == 4 && Cols == 4 && std::is_floating_point_v<T>) {
        return MakeRotation(FQuat::MakeFromRotator(rotation));
    }

    [[nodiscard]] constexpr T& operator()(SIZE_T row, SIZE_T column) noexcept {
        return Data[(row * Cols) + column];
    }

    [[nodiscard]] constexpr const T& operator()(SIZE_T row, SIZE_T column) const noexcept {
        return Data[(row * Cols) + column];
    }

    [[nodiscard]] constexpr FRowProxy operator[](SIZE_T row) noexcept {
        return FRowProxy{Data.data() + (row * Cols)};
    }

    [[nodiscard]] constexpr FConstRowProxy operator[](SIZE_T row) const noexcept {
        return FConstRowProxy{Data.data() + (row * Cols)};
    }

    [[nodiscard]] constexpr T* GetData() noexcept { return Data.data(); }
    [[nodiscard]] constexpr const T* GetData() const noexcept { return Data.data(); }

    [[nodiscard]] constexpr std::array<T, Cols> GetRow(SIZE_T row) const noexcept {
        std::array<T, Cols> result{};
        for (SIZE_T column = 0; column < Cols; ++column) {
            result[column] = (*this)(row, column);
        }
        return result;
    }

    [[nodiscard]] constexpr std::array<T, Rows> GetColumn(SIZE_T column) const noexcept {
        std::array<T, Rows> result{};
        for (SIZE_T row = 0; row < Rows; ++row) {
            result[row] = (*this)(row, column);
        }
        return result;
    }

    constexpr void SetRow(SIZE_T row, const std::array<T, Cols>& values) noexcept {
        for (SIZE_T column = 0; column < Cols; ++column) {
            (*this)(row, column) = values[column];
        }
    }

    constexpr void SetColumn(SIZE_T column, const std::array<T, Rows>& values) noexcept {
        for (SIZE_T row = 0; row < Rows; ++row) {
            (*this)(row, column) = values[row];
        }
    }

    [[nodiscard]] constexpr TVector3<T> GetOrigin() const noexcept requires (Rows >= 3 && Cols == 4) {
        return {(*this)(0, 3), (*this)(1, 3), (*this)(2, 3)};
    }

    constexpr void SetOrigin(const TVector3<T>& origin) noexcept requires (Rows >= 3 && Cols == 4) {
        (*this)(0, 3) = origin.X;
        (*this)(1, 3) = origin.Y;
        (*this)(2, 3) = origin.Z;
    }

    [[nodiscard]] constexpr TMatrix operator+(const TMatrix& rhs) const noexcept {
        TMatrix result;
        for (SIZE_T index = 0; index < ElementCount; ++index) {
            result.Data[index] = Data[index] + rhs.Data[index];
        }
        return result;
    }

    [[nodiscard]] constexpr TMatrix operator-(const TMatrix& rhs) const noexcept {
        TMatrix result;
        for (SIZE_T index = 0; index < ElementCount; ++index) {
            result.Data[index] = Data[index] - rhs.Data[index];
        }
        return result;
    }

    [[nodiscard]] constexpr TMatrix operator*(T scalar) const noexcept {
        TMatrix result;
        for (SIZE_T index = 0; index < ElementCount; ++index) {
            result.Data[index] = Data[index] * scalar;
        }
        return result;
    }

    [[nodiscard]] constexpr TMatrix operator/(T scalar) const noexcept {
        TMatrix result;
        for (SIZE_T index = 0; index < ElementCount; ++index) {
            result.Data[index] = Data[index] / scalar;
        }
        return result;
    }

    [[nodiscard]] constexpr TMatrix HadamardProduct(const TMatrix& rhs) const noexcept {
        TMatrix result;
        for (SIZE_T index = 0; index < ElementCount; ++index) {
            result.Data[index] = Data[index] * rhs.Data[index];
        }
        return result;
    }

    TMatrix& operator+=(const TMatrix& rhs) noexcept {
        for (SIZE_T index = 0; index < ElementCount; ++index) {
            Data[index] += rhs.Data[index];
        }
        return *this;
    }

    TMatrix& operator-=(const TMatrix& rhs) noexcept {
        for (SIZE_T index = 0; index < ElementCount; ++index) {
            Data[index] -= rhs.Data[index];
        }
        return *this;
    }

    TMatrix& operator*=(T scalar) noexcept {
        for (T& value : Data) {
            value *= scalar;
        }
        return *this;
    }

    template <SIZE_T OtherCols>
    [[nodiscard]] constexpr TMatrix<T, Rows, OtherCols> operator*(const TMatrix<T, Cols, OtherCols>& rhs) const noexcept {
        TMatrix<T, Rows, OtherCols> result;
        for (SIZE_T row = 0; row < Rows; ++row) {
            for (SIZE_T column = 0; column < OtherCols; ++column) {
                T value = T(0);
                for (SIZE_T index = 0; index < Cols; ++index) {
                    value += (*this)(row, index) * rhs(index, column);
                }
                result(row, column) = value;
            }
        }
        return result;
    }

    [[nodiscard]] constexpr TVector4<T> operator*(const TVector4<T>& rhs) const noexcept requires (Cols == 4) {
        TVector4<T> result{};
        for (SIZE_T row = 0; row < Rows && row < 4; ++row) {
            result[row] =
                ((*this)(row, 0) * rhs.X) +
                ((*this)(row, 1) * rhs.Y) +
                ((*this)(row, 2) * rhs.Z) +
                ((*this)(row, 3) * rhs.W);
        }
        return result;
    }

    [[nodiscard]] constexpr TVector3<T> operator*(const TVector3<T>& rhs) const noexcept requires (Rows == 3 && Cols == 3) {
        return {
            ((*this)(0, 0) * rhs.X) + ((*this)(0, 1) * rhs.Y) + ((*this)(0, 2) * rhs.Z),
            ((*this)(1, 0) * rhs.X) + ((*this)(1, 1) * rhs.Y) + ((*this)(1, 2) * rhs.Z),
            ((*this)(2, 0) * rhs.X) + ((*this)(2, 1) * rhs.Y) + ((*this)(2, 2) * rhs.Z)};
    }

    [[nodiscard]] constexpr TMatrix<T, Cols, Rows> Transpose() const noexcept {
        TMatrix<T, Cols, Rows> result;
        for (SIZE_T row = 0; row < Rows; ++row) {
            for (SIZE_T column = 0; column < Cols; ++column) {
                result(column, row) = (*this)(row, column);
            }
        }
        return result;
    }

    [[nodiscard]] constexpr T Trace() const noexcept requires (IsSquare) {
        T result = T(0);
        for (SIZE_T index = 0; index < Rows; ++index) {
            result += (*this)(index, index);
        }
        return result;
    }

    [[nodiscard]] T Determinant() const noexcept requires (IsSquare && std::is_floating_point_v<T>) {
        TMatrix temp(*this);
        T determinant = T(1);

        for (SIZE_T pivotColumn = 0; pivotColumn < Rows; ++pivotColumn) {
            SIZE_T pivotRow = pivotColumn;
            T pivotMagnitude = FMath::Abs(temp(pivotRow, pivotColumn));

            for (SIZE_T row = pivotColumn + 1; row < Rows; ++row) {
                const T candidateMagnitude = FMath::Abs(temp(row, pivotColumn));
                if (candidateMagnitude > pivotMagnitude) {
                    pivotMagnitude = candidateMagnitude;
                    pivotRow = row;
                }
            }

            if (FMath::IsNearlyZero(static_cast<FReal>(pivotMagnitude), FMath::SmallNumber)) {
                return T(0);
            }

            if (pivotRow != pivotColumn) {
                for (SIZE_T column = 0; column < Cols; ++column) {
                    std::swap(temp(pivotColumn, column), temp(pivotRow, column));
                }
                determinant = -determinant;
            }

            const T pivotValue = temp(pivotColumn, pivotColumn);
            determinant *= pivotValue;

            for (SIZE_T row = pivotColumn + 1; row < Rows; ++row) {
                const T factor = temp(row, pivotColumn) / pivotValue;
                if (FMath::IsNearlyZero(static_cast<FReal>(factor), FMath::SmallNumber)) {
                    continue;
                }

                for (SIZE_T column = pivotColumn; column < Cols; ++column) {
                    temp(row, column) -= factor * temp(pivotColumn, column);
                }
            }
        }

        return determinant;
    }

    [[nodiscard]] TOptional<TMatrix> Inverse() const noexcept requires (IsSquare && std::is_floating_point_v<T>) {
        TMatrix left(*this);
        TMatrix right = TMatrix::Identity();

        for (SIZE_T pivotColumn = 0; pivotColumn < Rows; ++pivotColumn) {
            SIZE_T pivotRow = pivotColumn;
            T pivotMagnitude = FMath::Abs(left(pivotRow, pivotColumn));

            for (SIZE_T row = pivotColumn + 1; row < Rows; ++row) {
                const T candidateMagnitude = FMath::Abs(left(row, pivotColumn));
                if (candidateMagnitude > pivotMagnitude) {
                    pivotMagnitude = candidateMagnitude;
                    pivotRow = row;
                }
            }

            if (FMath::IsNearlyZero(static_cast<FReal>(pivotMagnitude), FMath::SmallNumber)) {
                return std::nullopt;
            }

            if (pivotRow != pivotColumn) {
                for (SIZE_T column = 0; column < Cols; ++column) {
                    std::swap(left(pivotColumn, column), left(pivotRow, column));
                    std::swap(right(pivotColumn, column), right(pivotRow, column));
                }
            }

            const T pivotValue = left(pivotColumn, pivotColumn);
            for (SIZE_T column = 0; column < Cols; ++column) {
                left(pivotColumn, column) /= pivotValue;
                right(pivotColumn, column) /= pivotValue;
            }

            for (SIZE_T row = 0; row < Rows; ++row) {
                if (row == pivotColumn) {
                    continue;
                }

                const T factor = left(row, pivotColumn);
                if (FMath::IsNearlyZero(static_cast<FReal>(factor), FMath::SmallNumber)) {
                    continue;
                }

                for (SIZE_T column = 0; column < Cols; ++column) {
                    left(row, column) -= factor * left(pivotColumn, column);
                    right(row, column) -= factor * right(pivotColumn, column);
                }
            }
        }

        return right;
    }

    [[nodiscard]] constexpr TVector3<T> TransformPosition(const TVector3<T>& value) const noexcept requires (Rows >= 3 && Cols == 4) {
        return {
            ((*this)(0, 0) * value.X) + ((*this)(0, 1) * value.Y) + ((*this)(0, 2) * value.Z) + (*this)(0, 3),
            ((*this)(1, 0) * value.X) + ((*this)(1, 1) * value.Y) + ((*this)(1, 2) * value.Z) + (*this)(1, 3),
            ((*this)(2, 0) * value.X) + ((*this)(2, 1) * value.Y) + ((*this)(2, 2) * value.Z) + (*this)(2, 3)};
    }

    [[nodiscard]] constexpr TVector3<T> TransformPoint(const TVector3<T>& value) const noexcept requires (Rows >= 3 && Cols == 4) {
        return TransformPosition(value);
    }

    [[nodiscard]] constexpr TVector3<T> TransformVector(const TVector3<T>& value) const noexcept requires (Rows >= 3 && Cols >= 3) {
        return {
            ((*this)(0, 0) * value.X) + ((*this)(0, 1) * value.Y) + ((*this)(0, 2) * value.Z),
            ((*this)(1, 0) * value.X) + ((*this)(1, 1) * value.Y) + ((*this)(1, 2) * value.Z),
            ((*this)(2, 0) * value.X) + ((*this)(2, 1) * value.Y) + ((*this)(2, 2) * value.Z)};
    }

private:
    std::array<T, ElementCount> Data{};
};

template <typename T, SIZE_T Rows, SIZE_T Cols>
inline std::ostream& operator<<(std::ostream& stream, const TMatrix<T, Rows, Cols>& value) {
    stream << "[";
    for (SIZE_T row = 0; row < Rows; ++row) {
        if (row != 0) {
            stream << " ";
        }
        stream << "[";
        for (SIZE_T column = 0; column < Cols; ++column) {
            if (column != 0) {
                stream << ", ";
            }
            stream << value(row, column);
        }
        stream << "]";
    }
    stream << "]";
    return stream;
}

using FMatrix3x3 = TMatrix<FReal, 3, 3>;
using FMatrix44f = TMatrix<float32, 4, 4>;
using FMatrix44d = TMatrix<FReal, 4, 4>;
using FMatrix = FMatrix44d;

template <typename T, SIZE_T Rows, SIZE_T Cols>
[[nodiscard]] constexpr TMatrix<T, Rows, Cols> operator*(T scalar, const TMatrix<T, Rows, Cols>& matrix) noexcept {
    return matrix * scalar;
}
