//
// Created by IDKTHIS on 20.05.2026.
//

#include "VoxCore/FApplication.h"



bool VoxCore::FApplication::Initialize() {
    m_IsRunning = true;
    m_EngineContext = std::make_unique<FEngineContext>();



    return true;
}

void VoxCore::FApplication::Run() {
    while (m_IsRunning) {
        PollEvents();
    }
}

void VoxCore::FApplication::Shutdown() {
    m_IsRunning = false;
}

FEngineContext * VoxCore::FApplication::GetEngineContext() const {
    return m_EngineContext.get();
}

void VoxCore::FApplication::PollEvents() {
    for (auto& window : m_EngineContext->GetWindows()) {
        if (!window) continue;
        window->PollEvents();
    }
}
