#include "VoxEngine/FEngineContext.h"

#include "VoxCore/Reflection/ModuleRegistration.h"
#include "VoxEngine/Reflection/ModuleRegistration.h"

FEngineContext::FEngineContext() : m_MainWindowIndex(0) {
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

std::optional<uint8> FEngineContext::AddWindow(VoxCore::Window::IWindow *window) noexcept {
    if (!window) {
        return std::nullopt;
    }
    if (const auto Index = GetWindowIndex(window)) return Index;
    m_Windows.Emplace(window);
    return static_cast<uint8>(m_Windows.Num() - 1);
}

void FEngineContext::RemoveWindow(VoxCore::Window::IWindow *window) noexcept {
    if (!window) return;

    auto indexOpt = GetWindowIndex(window);
    if (!indexOpt) return;

    uint8 index = *indexOpt;
    m_Windows.RemoveAt(index);

    if (m_Windows.IsEmpty()) {
        m_MainWindowIndex = 0;
        return;
    }

    if (m_MainWindowIndex == index) {
        m_MainWindowIndex = 0;
    } else if (m_MainWindowIndex > index) {
        --m_MainWindowIndex;
    }
}
bool FEngineContext::HasWindow(VoxCore::Window::IWindow *window) noexcept {
    return GetWindowIndex(window).has_value();
}

std::optional<uint8> FEngineContext::GetWindowIndex(VoxCore::Window::IWindow *window) noexcept {
    for (typename decltype(m_Windows)::SizeType i = 0; i < m_Windows.Num(); ++i) {
        if (m_Windows[i].get() == window) {
            return static_cast<uint8>(i);
        }
    }
    return std::nullopt;
}

void FEngineContext::SetWindowContext(uint8 windowIndex) noexcept {
    if (windowIndex < m_Windows.Num()) {
        m_MainWindowIndex = windowIndex;
    }
}

const TArray<TUniquePtr<VoxCore::Window::IWindow>> & FEngineContext::GetWindows() const noexcept {
    return m_Windows;
}


VoxCore::Window::IWindow* FEngineContext::GetWindowContext() noexcept {
    if (m_MainWindowIndex < m_Windows.Num()) {
        return m_Windows[m_MainWindowIndex].get();
    }

    if (!m_Windows.IsEmpty()) {
        m_MainWindowIndex = 0;
        return m_Windows[0].get();
    }

    return nullptr;
}
