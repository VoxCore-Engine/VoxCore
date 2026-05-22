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
    std::optional<uint8> AddWindow(VoxCore::Window::IWindow* window) noexcept;
    void RemoveWindow(VoxCore::Window::IWindow* window) noexcept;
    [[nodiscard]] bool HasWindow(VoxCore::Window::IWindow* window) noexcept;
    [[nodiscard]] std::optional<uint8> GetWindowIndex(VoxCore::Window::IWindow* window) noexcept;
    void SetWindowContext(uint8 windowIndex) noexcept;
    const TArray<TUniquePtr<VoxCore::Window::IWindow>>& GetWindows() const noexcept;
    VoxCore::Window::IWindow* GetWindowContext() noexcept;
private:
    FCVarManager m_CVarManager;
    FReflectionRegistry m_ReflectionRegistry;
    uint8 m_MainWindowIndex;
    TArray<TUniquePtr<VoxCore::Window::IWindow>> m_Windows;
};
