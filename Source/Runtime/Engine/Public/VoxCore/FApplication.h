//
// Created by IDKTHIS on 20.05.2026.
//

#pragma once
#include "VoxEngine/FEngineContext.h"

namespace VoxCore {
    class FApplication {
    public:
        virtual ~FApplication() = default;

        FApplication() = default;

        virtual bool Initialize();
        virtual void Run();
        virtual void Shutdown();
        FEngineContext* GetEngineContext() const;
    protected:
        virtual void PollEvents();

        TUniquePtr<FEngineContext> m_EngineContext;
        bool m_IsRunning = false;
    };

}
