#pragma once

#include <ostream>
#include <string>
#include <string_view>
#include <utility>

#include "VoxCore/Core/Types/Common.h"

class FString {
public:
    using StorageType = std::string;
    using SizeType = StorageType::size_type;

    FString() = default;
    FString(const char* value) : m_Value(value ? value : "") {}
    FString(std::string_view value) : m_Value(value) {}
    FString(const StorageType& value) : m_Value(value) {}
    FString(StorageType&& value) noexcept : m_Value(std::move(value)) {}

    [[nodiscard]] const char* c_str() const noexcept { return m_Value.c_str(); } // NOLINT(*-identifier-naming)
    [[nodiscard]] const char* Data() const noexcept { return m_Value.data(); }
    [[nodiscard]] char* Data() noexcept { return m_Value.data(); }
    [[nodiscard]] SizeType Len() const noexcept { return m_Value.size(); }
    [[nodiscard]] bool IsEmpty() const noexcept { return m_Value.empty(); }
    [[nodiscard]] std::string_view View() const noexcept { return m_Value; }
    [[nodiscard]] const StorageType& Std() const noexcept { return m_Value; }
    [[nodiscard]] StorageType& Std() noexcept { return m_Value; }

    void Reserve(SizeType count) { m_Value.reserve(count); }
    void Reset() { m_Value.clear(); }

    FString& Append(std::string_view suffix) {
        m_Value.append(suffix);
        return *this;
    }

    FString& operator+=(std::string_view suffix) { return Append(suffix); }
    FString& operator+=(const FString& suffix) { return Append(suffix.View()); }
    FString& operator+=(const char* suffix) { return Append(suffix ? suffix : ""); }

    [[nodiscard]] char& operator[](SizeType index) noexcept { return m_Value[index]; }
    [[nodiscard]] const char& operator[](SizeType index) const noexcept { return m_Value[index]; }

    [[nodiscard]] auto begin() noexcept { return m_Value.begin(); }
    [[nodiscard]] auto begin() const noexcept { return m_Value.begin(); }
    [[nodiscard]] auto end() noexcept { return m_Value.end(); }
    [[nodiscard]] auto end() const noexcept { return m_Value.end(); }
    [[nodiscard]] auto Begin() noexcept { return m_Value.begin(); }
    [[nodiscard]] auto Begin() const noexcept { return m_Value.begin(); }
    [[nodiscard]] auto End() noexcept { return m_Value.end(); }
    [[nodiscard]] auto End() const noexcept { return m_Value.end(); }

    [[nodiscard]] explicit operator std::string_view() const noexcept { return View(); }

    friend bool operator==(const FString& lhs, const FString& rhs) noexcept { return lhs.m_Value == rhs.m_Value; }
    friend bool operator!=(const FString& lhs, const FString& rhs) noexcept { return !(lhs == rhs); }
    friend bool operator==(const FString& lhs, std::string_view rhs) noexcept { return lhs.View() == rhs; }
    friend bool operator==(std::string_view lhs, const FString& rhs) noexcept { return lhs == rhs.View(); }
    friend bool operator!=(const FString& lhs, std::string_view rhs) noexcept { return !(lhs == rhs); }
    friend bool operator!=(std::string_view lhs, const FString& rhs) noexcept { return !(lhs == rhs); }
    friend bool operator<(const FString& lhs, const FString& rhs) noexcept { return lhs.m_Value < rhs.m_Value; }

    friend FString operator+(const FString& lhs, const FString& rhs) {
        FString result(lhs);
        result += rhs;
        return result;
    }

    friend std::ostream& operator<<(std::ostream& stream, const FString& value) {
        stream << value.m_Value;
        return stream;
    }

private:
    StorageType m_Value;
};

using FStringView = std::string_view;
using FName = FString;
using FText = FString;

namespace std {
    template <>
    struct hash<FString> {
        SIZE_T operator()(const FString& value) const noexcept {
            return std::hash<std::string_view>{}(value.View());
        }
    };
}
