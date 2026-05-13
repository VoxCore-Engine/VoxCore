#include "VoxEngine/FEngineContext.h"

#include "VoxCore/Reflection/ModuleRegistration.h"
#include "VoxEngine/Reflection/ModuleRegistration.h"

FEngineContext::FEngineContext() {
    RegisterVoxCoreTypes(m_ReflectionRegistry);
    RegisterVoxEngineTypes(m_ReflectionRegistry);
}

FCVarManager& FEngineContext::GetCVarManager() noexcept {
    return m_CVarManager;
}

const FCVarManager& FEngineContext::GetCVarManager() const noexcept {
    return m_CVarManager;
}

FReflectionRegistry& FEngineContext::GetReflectionRegistry() noexcept {
    return m_ReflectionRegistry;
}

const FReflectionRegistry& FEngineContext::GetReflectionRegistry() const noexcept {
    return m_ReflectionRegistry;
}
