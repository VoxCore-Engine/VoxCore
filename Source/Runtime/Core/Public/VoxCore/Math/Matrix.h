//
// Created by IDKTHIS on 12.05.2026.
//

#pragma once
#include <array>
#include <initializer_list>
#include <ostream>
#include <type_traits>
#include <utility>

#include "VoxCore/Core/Types/Common.h"
#include "VoxCore/Math/Vector.h"


template <typename T, SIZE_T Rows, SIZE_T Cols>
class TMatrixStorage {
public:
    std::array<T, Rows * Cols> Data{};
};

template <typename T, SIZE_T Rows, SIZE_T Cols>
class TMatrix : public TMatrixStorage<T, Rows, Cols> {
public:
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
    explicit constexpr TMatrix(const std::array<T, ElementCount>& values)
        : TMatrixStorage<T, Rows, Cols>{values} {}

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

    [[nodiscard]] constexpr T& operator()(SIZE_T row, SIZE_T column) noexcept {
        return this->Data[(row * Cols) + column];
    }

    [[nodiscard]] constexpr const T& operator()(SIZE_T row, SIZE_T column) const noexcept {
        return this->Data[(row * Cols) + column];
    }

    [[nodiscard]] constexpr FRowProxy operator[](SIZE_T row) noexcept {
        return FRowProxy{this->Data.data() + (row * Cols)};
    }

    [[nodiscard]] constexpr FConstRowProxy operator[](SIZE_T row) const noexcept {
        return FConstRowProxy{this->Data.data() + (row * Cols)};
    }

    [[nodiscard]] constexpr T* GetData() noexcept { return this->Data.data(); }
    [[nodiscard]] constexpr const T* GetData() const noexcept { return this->Data.data(); }

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

    [[nodiscard]] constexpr TMatrix operator+(const TMatrix& rhs) const noexcept {
        TMatrix result;
        for (SIZE_T index = 0; index < ElementCount; ++index) {
            result.GetData()[index] = this->Data[index] + rhs.GetData()[index];
        }
        return result;
    }

    [[nodiscard]] constexpr TMatrix operator-(const TMatrix& rhs) const noexcept {
        TMatrix result;
        for (SIZE_T index = 0; index < ElementCount; ++index) {
            result.GetData()[index] = this->Data[index] - rhs.GetData()[index];
        }
        return result;
    }

    [[nodiscard]] constexpr TMatrix operator*(T scalar) const noexcept {
        TMatrix result;
        for (SIZE_T index = 0; index < ElementCount; ++index) {
            result.GetData()[index] = this->Data[index] * scalar;
        }
        return result;
    }

    [[nodiscard]] constexpr TMatrix operator/(T scalar) const noexcept {
        TMatrix result;
        for (SIZE_T index = 0; index < ElementCount; ++index) {
            result.GetData()[index] = this->Data[index] / scalar;
        }
        return result;
    }

    [[nodiscard]] constexpr TMatrix HadamardProduct(const TMatrix& rhs) const noexcept {
        TMatrix result;
        for (SIZE_T index = 0; index < ElementCount; ++index) {
            result.GetData()[index] = this->Data[index] * rhs.GetData()[index];
        }
        return result;
    }

    TMatrix& operator+=(const TMatrix& rhs) noexcept {
        for (SIZE_T index = 0; index < ElementCount; ++index) {
            this->Data[index] += rhs.GetData()[index];
        }
        return *this;
    }

    TMatrix& operator-=(const TMatrix& rhs) noexcept {
        for (SIZE_T index = 0; index < ElementCount; ++index) {
            this->Data[index] -= rhs.GetData()[index];
        }
        return *this;
    }

    TMatrix& operator*=(T scalar) noexcept {
        for (T& value : this->Data) {
            value *= scalar;
        }
        return *this;
    }

    template <SIZE_T OtherCols>
    [[nodiscard]] TMatrix<T, Rows, OtherCols> operator*(const TMatrix<T, Cols, OtherCols>& rhs) const noexcept {
        if constexpr (std::is_same_v<T, FReal> && Rows == 4 && Cols == 4 && OtherCols == 4) {
            TMatrix<T, Rows, OtherCols> result;
            core::math::intrinsics::MultiplyMatrix4x4(this->GetData(), rhs.GetData(), result.GetData());
            return result;
        } else {
            TMatrix<T, Rows, OtherCols> result;
            for (SIZE_T row = 0; row < Rows; ++row) {
                for (SIZE_T column = 0; column < OtherCols; ++column) {
                    T value = T(0);
                    for (SIZE_T i = 0; i < Cols; ++i) {
                        value += (*this)(row, i) * rhs(i, column);
                    }
                    result(row, column) = value;
                }
            }
            return result;
        }
    }

    [[nodiscard]] TVector4<T> operator*(const TVector4<T>& rhs) const noexcept requires (Cols == 4) {
        TVector4<T> result{};
        const T vector[4] = {rhs.X, rhs.Y, rhs.Z, rhs.W};
        T output[4] = {};
        for (SIZE_T row = 0; row < Rows && row < 4; ++row) {
            for (SIZE_T column = 0; column < 4; ++column) {
                output[row] += (*this)(row, column) * vector[column];
            }
        }
        if constexpr (Rows > 0) result.X = output[0];
        if constexpr (Rows > 1) result.Y = output[1];
        if constexpr (Rows > 2) result.Z = output[2];
        if constexpr (Rows > 3) result.W = output[3];
        return result;
    }

    [[nodiscard]] TVector3<T> operator*(const TVector3<T>& rhs) const noexcept requires (Rows == 3 && Cols == 3) {
        return {
            ((*this)(0, 0) * rhs.X) + ((*this)(0, 1) * rhs.Y) + ((*this)(0, 2) * rhs.Z),
            ((*this)(1, 0) * rhs.X) + ((*this)(1, 1) * rhs.Y) + ((*this)(1, 2) * rhs.Z),
            ((*this)(2, 0) * rhs.X) + ((*this)(2, 1) * rhs.Y) + ((*this)(2, 2) * rhs.Z)};
    }

    [[nodiscard]] TMatrix<T, Cols, Rows> Transpose() const noexcept {
        TMatrix<T, Cols, Rows> result;
        for (SIZE_T row = 0; row < Rows; ++row) {
            for (SIZE_T column = 0; column < Cols; ++column) {
                result(column, row) = (*this)(row, column);
            }
        }
        return result;
    }

    [[nodiscard]] T Trace() const noexcept requires (IsSquare) {
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

    [[nodiscard]] TVector3<T> TransformPosition(const TVector3<T>& value) const noexcept requires (Rows >= 3 && Cols == 4) {
        if constexpr (std::is_same_v<T, FReal> && Rows == 4) {
            const FReal input[3] = {value.X, value.Y, value.Z};
            FReal output[3] = {};
            core::math::intrinsics::TransformPosition4x4(this->GetData(), input, output);
            return {output[0], output[1], output[2]};
        } else {
            return {
                ((*this)(0, 0) * value.X) + ((*this)(0, 1) * value.Y) + ((*this)(0, 2) * value.Z) + (*this)(0, 3),
                ((*this)(1, 0) * value.X) + ((*this)(1, 1) * value.Y) + ((*this)(1, 2) * value.Z) + (*this)(1, 3),
                ((*this)(2, 0) * value.X) + ((*this)(2, 1) * value.Y) + ((*this)(2, 2) * value.Z) + (*this)(2, 3)};
        }
    }

    [[nodiscard]] TVector3<T> TransformPoint(const TVector3<T>& value) const noexcept requires (Rows >= 3 && Cols == 4) {
        return TransformPosition(value);
    }

    [[nodiscard]] TVector3<T> TransformVector(const TVector3<T>& value) const noexcept requires (Rows >= 3 && Cols >= 3) {
        if constexpr (std::is_same_v<T, FReal> && Rows == 4 && Cols == 4) {
            const FReal input[3] = {value.X, value.Y, value.Z};
            FReal output[3] = {};
            core::math::intrinsics::TransformVector4x4(this->GetData(), input, output);
            return {output[0], output[1], output[2]};
        } else {
            return {
                ((*this)(0, 0) * value.X) + ((*this)(0, 1) * value.Y) + ((*this)(0, 2) * value.Z),
                ((*this)(1, 0) * value.X) + ((*this)(1, 1) * value.Y) + ((*this)(1, 2) * value.Z),
                ((*this)(2, 0) * value.X) + ((*this)(2, 1) * value.Y) + ((*this)(2, 2) * value.Z)};
        }
    }
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

template <SIZE_T Rows, SIZE_T Cols, typename T = FReal>
using FMatrix = TMatrix<T, Rows, Cols>;

template <typename T, SIZE_T Rows, SIZE_T Cols>
[[nodiscard]] constexpr TMatrix<T, Rows, Cols> operator*(T scalar, const TMatrix<T, Rows, Cols>& matrix) noexcept {
    return matrix * scalar;
}