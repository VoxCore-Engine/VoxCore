#include "VoxCraft/VoxCraftProject.h"

#include "VoxCraft/ModuleRegistration.h"
#include "VoxCore/Platform/Window/VoxSDLWindow.h"

namespace {
    constexpr int32 GDefaultWindowWidth = 1920;
    constexpr int32 GDefaultWindowHeight = 1080;
}

bool VoxCraft::FVoxCraftProject::Initialize(FEngineContext& engineContext) {
    RegisterVoxCraftTypes(engineContext.GetReflectionRegistry());

    if (m_MainWindowIndex.has_value()) {
        return true;
    }

    const TOptional<uint8> windowIndex = engineContext.AddWindow(new VoxCore::Platform::Window::VoxSDLWindow());
    if (!windowIndex.has_value()) {
        return false;
    }

    engineContext.SetWindowContext(*windowIndex);
    VoxCore::Window::IWindow* const window = engineContext.GetWindowContext();
    if (window == nullptr) {
        return false;
    }

    if (!window->Create(GDefaultWindowWidth, GDefaultWindowHeight, GetProjectName())) {
        engineContext.RemoveWindow(window);
        return false;
    }

    m_MainWindowIndex = windowIndex;
    return true;
}

void VoxCraft::FVoxCraftProject::Shutdown(FEngineContext& engineContext) noexcept {
    if (!m_MainWindowIndex.has_value()) {
        return;
    }

    const uint8 windowIndex = *m_MainWindowIndex;
    const auto& windows = engineContext.GetWindows();
    if (windowIndex < windows.Num()) {
        engineContext.RemoveWindow(windows[windowIndex].get());
    }

    m_MainWindowIndex.reset();
}
