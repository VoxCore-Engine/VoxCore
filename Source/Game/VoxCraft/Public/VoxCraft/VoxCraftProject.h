#pragma once

#include "VoxCore/CoreMinimal.h"
#include "VoxEngine/FEngineContext.h"

namespace VoxCraft {
    class FVoxCraftProject {
    public:
        [[nodiscard]] bool Initialize(FEngineContext& engineContext);
        void Shutdown(FEngineContext& engineContext) noexcept;

        [[nodiscard]] static constexpr const char* GetProjectName() noexcept {
            return "VoxCraft";
        }

    private:
        TOptional<uint8> m_MainWindowIndex;
    };
}
