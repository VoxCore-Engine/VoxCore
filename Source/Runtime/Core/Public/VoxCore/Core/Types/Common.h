//
// Created by IDKTHIS on 10.05.2026.
//

#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>

using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using SIZE_T = std::size_t;

template <typename T>
using TOptional = std::optional<T>;

template <typename T>
using TSpan = std::span<T>;

template <typename T>
using TConstSpan = std::span<const T>;