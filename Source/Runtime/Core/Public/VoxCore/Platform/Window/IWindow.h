//
// Created by IDKTHIS on 13.05.2026.
//

#pragma once
#include <cstdint>
#include <string>


namespace VoxCore::Window {
    class IWindow {
    public:
        virtual ~IWindow() = default;

        virtual bool Create(const int width, const int height, const std::string& title) {
            m_Width = width;
            m_Height = height;
            return true;
        };
        virtual void PollEvents() = 0;
        virtual void SwapBuffers() = 0;
        virtual bool ShouldClose() const = 0;

        virtual void* GetNativeHandle() = 0;

        virtual void SetRelativeMouseMode(bool enable) = 0;
        virtual void ToggleRelativeMouseMode() = 0;
        virtual bool IsRelativeMouseMode() const { return m_RelativeMouseMode; }
        virtual void SetTitle(const std::string& title) = 0;
        [[nodiscard]] uint32_t GetWidth() const { return m_Width; }
        [[nodiscard]] uint32_t GetHeight() const { return m_Height; }
    protected:
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        bool m_RelativeMouseMode = false;
    };
}
