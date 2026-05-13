#pragma once

#include "VoxCore/CoreMinimal.h"
#include "VoxCore/Platform/Window/IWindow.h"
#include "VoxEngine/CVar/FCVar.h"
#include "VoxEngine/Reflection/FReflectionRegistry.h"

class FEngineContext {
public:
    FEngineContext();
    [[nodiscard]] FCVarManager& GetCVarManager() noexcept;
    [[nodiscard]] const FCVarManager& GetCVarManager() const noexcept;
    [[nodiscard]] FReflectionRegistry& GetReflectionRegistry() noexcept;
    [[nodiscard]] const FReflectionRegistry& GetReflectionRegistry() const noexcept;
    
private:
    FCVarManager m_CVarManager;
    FReflectionRegistry m_ReflectionRegistry;
    TUniquePtr<VoxCore::Window::IWindow> m_Window;
};
