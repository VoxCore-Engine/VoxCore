#pragma once

#include "VoxCore/CoreMinimal.h"
#include "VoxCore/Reflection/IReflectionRegistry.h"

class UClass;

class FReflectionRegistry final : public IReflectionRegistry {
public:
    void RegisterClass(UClass* cls) override;

    [[nodiscard]] const UClass* FindClass(FStringView name) const;
    [[nodiscard]] TSpan<UClass* const> GetRegisteredClasses() const noexcept;
    void Reset();

private:
    TArray<UClass*> RegisteredClasses;
    TMap<FString, UClass*> RegisteredClassesByName;
};
