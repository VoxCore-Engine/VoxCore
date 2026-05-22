//
// Created by IDKTHIS on 22.05.2026.
//

#pragma once
#include <cstddef>


namespace VoxCore::Memory {
    template <typename T>
    struct TAllocator {
        using value_type = T;

        TAllocator() = default;

        template <typename U>
        TAllocator(const TAllocator<U>&) {}

        T* allocate(std::size_t count) {
            return static_cast<T*>(::operator new(count * sizeof(T)));
        }

        void deallocate(T* ptr, std::size_t) noexcept {
            ::operator delete(ptr);
        }
    };
    template <typename T, typename U>
    bool operator==(const TAllocator<T>&, const TAllocator<U>&) {
        return true;
    }

    template <typename T, typename U>
    bool operator!=(const TAllocator<T>&, const TAllocator<U>&) {
        return false;
    }


}
