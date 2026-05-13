#pragma once

#include <functional>
#include <unordered_map>

#include "VoxCore/Core/Types/Common.h"

template <
    typename KeyType,
    typename ValueType,
    typename Hasher = std::hash<KeyType>,
    typename KeyEqual = std::equal_to<KeyType>,
    typename Allocator = std::allocator<std::pair<const KeyType, ValueType>>>
class TMap {
public:
    using StorageType = std::unordered_map<KeyType, ValueType, Hasher, KeyEqual, Allocator>;
    using Iterator = typename StorageType::iterator;
    using ConstIterator = typename StorageType::const_iterator;

    [[nodiscard]] bool IsEmpty() const noexcept { return Items.empty(); }
    [[nodiscard]] SIZE_T Num() const noexcept { return Items.size(); }
    [[nodiscard]] SIZE_T Size() const noexcept { return Items.size(); }

    ValueType& Add(const KeyType& key, const ValueType& value) {
        return Items.insert_or_assign(key, value).first->second;
    }

    ValueType& Add(KeyType&& key, ValueType&& value) {
        return Items.insert_or_assign(std::move(key), std::move(value)).first->second;
    }

    template <typename... Args>
    ValueType& Emplace(KeyType key, Args&&... args) {
        return Items.try_emplace(std::move(key), std::forward<Args>(args)...).first->second;
    }

    [[nodiscard]] bool Contains(const KeyType& key) const { return Items.contains(key); }

    [[nodiscard]] ValueType* Find(const KeyType& key) {
        if (auto it = Items.find(key); it != Items.end()) {
            return &it->second;
        }
        return nullptr;
    }

    [[nodiscard]] const ValueType* Find(const KeyType& key) const {
        if (auto it = Items.find(key); it != Items.end()) {
            return &it->second;
        }
        return nullptr;
    }

    bool Remove(const KeyType& key) { return Items.erase(key) != 0; }

    ValueType& operator[](const KeyType& key) { return Items[key]; }

    [[nodiscard]] Iterator begin() noexcept { return Items.begin(); }
    [[nodiscard]] ConstIterator begin() const noexcept { return Items.begin(); }
    [[nodiscard]] Iterator end() noexcept { return Items.end(); }
    [[nodiscard]] ConstIterator end() const noexcept { return Items.end(); }

private:
    StorageType Items;
};
