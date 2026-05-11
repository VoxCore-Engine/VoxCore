//
// Created by IDKTHIS on 10.05.2026.
//

#pragma once
#include <functional>
#include <unordered_set>

#include "Common.h"

template <
    typename ElementType,
    typename Hasher = std::hash<ElementType>,
    typename KeyEqual = std::equal_to<ElementType>,
    typename Allocator = std::allocator<ElementType>>
class TSet {
public:
    using StorageType = std::unordered_set<ElementType, Hasher, KeyEqual, Allocator>;
    using Iterator = typename StorageType::iterator;
    using ConstIterator = typename StorageType::const_iterator;

    [[nodiscard]] bool IsEmpty() const noexcept { return Items.empty(); }
    [[nodiscard]] SIZE_T Num() const noexcept { return Items.size(); }
    [[nodiscard]] SIZE_T size() const noexcept { return Items.size(); }

    bool Add(const ElementType& value) { return Items.insert(value).second; }
    bool Add(ElementType&& value) { return Items.insert(std::move(value)).second; }

    [[nodiscard]] bool Contains(const ElementType& value) const { return Items.contains(value); }
    bool Remove(const ElementType& value) { return Items.erase(value) != 0; }

    [[nodiscard]] Iterator Begin() noexcept { return Items.begin(); }
    [[nodiscard]] ConstIterator Begin() const noexcept { return Items.begin(); }
    [[nodiscard]] Iterator End() noexcept { return Items.end(); }
    [[nodiscard]] ConstIterator End() const noexcept { return Items.end(); }

private:
    StorageType Items;
};