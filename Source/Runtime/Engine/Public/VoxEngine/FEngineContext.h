#pragma once

#include "VoxCore/CoreMinimal.h"
#include "VoxEngine/Reflection/FReflectionRegistry.h"

class FEngineContext {
public:
    FEngineContext();

    [[nodiscard]] FReflectionRegistry& GetReflectionRegistry() noexcept;
    [[nodiscard]] const FReflectionRegistry& GetReflectionRegistry() const noexcept;

private:
    FReflectionRegistry ReflectionRegistry;
};
