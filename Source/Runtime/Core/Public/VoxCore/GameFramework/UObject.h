#pragma once

#include "VoxCore/Reflection/ObjectMacros.h"
#include "VoxCore/Reflection/UClass.h"
#include "VoxCore/GameFramework/UObject.generated.h"

UCLASS()
class UObject {
    GENERATED_BODY()
public:
    virtual ~UObject() = default;

    [[nodiscard]] UClass* GetSuperClass() const {
        return GetClass()->SuperClass;
    }

    template <typename T>
    [[nodiscard]] bool IsA() const {
        return GetClass()->IsA(T::StaticClass());
    }

    template <typename T>
    [[nodiscard]] T* Cast() {
        return IsA<T>() ? static_cast<T*>(this) : nullptr;
    }

    template <typename T>
    [[nodiscard]] const T* Cast() const {
        return IsA<T>() ? static_cast<const T*>(this) : nullptr;
    }
};
