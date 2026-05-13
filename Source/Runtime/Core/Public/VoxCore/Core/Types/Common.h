#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <type_traits>
#include <utility>

using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using float32 = float;
using float64 = double;
using ANSICHAR = char;
using SIZE_T = std::size_t;
using SSIZE_T = std::ptrdiff_t;

template <typename T>
using TOptional = std::optional<T>;

template <typename T>
using TSpan = std::span<T>;

template <typename T>
using TConstSpan = std::span<const T>;

template <typename T>
using TRemoveCVRef = std::remove_cvref_t<T>;

template <typename T>
[[nodiscard]] constexpr TOptional<TRemoveCVRef<T>> MakeOptional(T&& value) {
    return TOptional<TRemoveCVRef<T>>(std::forward<T>(value));
}
