#pragma once


#include <vector>

#include "VoxCore/Core/Types/Common.h"
#include "VoxCore/Memory/Allocator.h"

template <typename T, typename Allocator = VoxCore::Memory::TAllocator<T>>
class TArray {
public:
    using ElementType = T;
    using StorageType = std::vector<T, Allocator>;
    using SizeType = typename StorageType::size_type;
    using Iterator = typename StorageType::iterator;
    using ConstIterator = typename StorageType::const_iterator;

    TArray() = default;

    TArray(std::initializer_list<T> values) : Items(values) {}

    [[nodiscard]] bool IsEmpty() const noexcept { return Items.empty(); }
    [[nodiscard]] SizeType Num() const noexcept { return Items.size(); }
    [[nodiscard]] SizeType size() const noexcept { return Items.size(); }

    void Reserve(SizeType count) { Items.reserve(count); }
    void reserve(SizeType count) { Items.reserve(count); }
    void Reset() { Items.clear(); }

    T& Add(const T& value) {
        Items.push_back(value);
        return Items.back();
    }

    T& Add(T&& value) {
        Items.push_back(std::move(value));
        return Items.back();
    }

    template <typename... Args>
    T& Emplace(Args&&... args) {
        Items.emplace_back(std::forward<Args>(args)...);
        return Items.back();
    }

    T& push_back(const T& value) { return Add(value); }
    T& push_back(T&& value) { return Add(std::move(value)); }

    template <typename... Args>
    T& emplace_back(Args&&... args) {
        return Emplace(std::forward<Args>(args)...);
    }

    void RemoveAt(SizeType index) { Items.erase(Items.begin() + static_cast<std::ptrdiff_t>(index)); }
    Iterator erase(ConstIterator where) { return Items.erase(where); }

    [[nodiscard]] T* GetData() noexcept { return Items.data(); }
    [[nodiscard]] const T* GetData() const noexcept { return Items.data(); }
    [[nodiscard]] T* data() noexcept { return Items.data(); }
    [[nodiscard]] const T* data() const noexcept { return Items.data(); }
    [[nodiscard]] TSpan<T> AsSpan() noexcept { return {Items.data(), Items.size()}; }
    [[nodiscard]] TConstSpan<T> AsSpan() const noexcept { return {Items.data(), Items.size()}; }

    [[nodiscard]] T& operator[](SizeType index) noexcept { return Items[index]; }
    [[nodiscard]] const T& operator[](SizeType index) const noexcept { return Items[index]; }

    [[nodiscard]] Iterator begin() noexcept { return Items.begin(); }
    [[nodiscard]] ConstIterator begin() const noexcept { return Items.begin(); }
    [[nodiscard]] Iterator end() noexcept { return Items.end(); }
    [[nodiscard]] ConstIterator end() const noexcept { return Items.end(); }

private:
    StorageType Items;
};

template <typename T>
[[nodiscard]] TArray<T> MakeArray(std::initializer_list<T> values) {
    return TArray<T>(values);
}

template <typename T, typename... Args>
[[nodiscard]] TArray<T> MakeArray(Args&&... args) {
    TArray<T> values;
    values.Reserve(sizeof...(Args));
    (values.Add(T(std::forward<Args>(args))), ...);
    return values;
}

