#include "VoxEngine/FEngineContext.h"

#include "VoxCore/Reflection/ModuleRegistration.h"
#include "VoxEngine/Reflection/ModuleRegistration.h"

FEngineContext::FEngineContext() {
    RegisterVoxCoreTypes(ReflectionRegistry);
    RegisterVoxEngineTypes(ReflectionRegistry);
}

FReflectionRegistry& FEngineContext::GetReflectionRegistry() noexcept {
    return ReflectionRegistry;
}

const FReflectionRegistry& FEngineContext::GetReflectionRegistry() const noexcept {
    return ReflectionRegistry;
}
