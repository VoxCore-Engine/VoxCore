//
// Created by IDKTHIS on 13.05.2026.
//

#pragma once
#include "SDL3/SDL_video.h"
#include "VoxCore/Platform/Window/IWindow.h"


namespace VoxCore::Platform::Window {
    class VoxSDLWindow : public VoxCore::Window::IWindow {
        public:
        ~VoxSDLWindow();
        bool Create(int width, int height, const std::string &title) override;
        void PollEvents() override;

        void SwapBuffers() override {};

        bool ShouldClose() const override { return true; };

        void* GetNativeHandle() override {return m_Window;};

        void SetRelativeMouseMode(bool enable) override {};

        void ToggleRelativeMouseMode() override {};

        void SetTitle(const std::string &title) override{};

        private:
        SDL_Window* m_Window = nullptr;
    };
}

