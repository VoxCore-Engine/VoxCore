//
// Created by IDKTHIS on 13.05.2026.
//

#include "VoxCore/Platform/Window/VoxSDLWindow.h"
#include <SDL3/SDL.h>


using namespace VoxCore::Platform::Window;

VoxSDLWindow::~VoxSDLWindow() {
}

bool VoxSDLWindow::Create(int width, int height, const std::string &title) {
    if (!IWindow::Create(width, height, title)) {
        return false;
    }
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return false;
    }
    m_Window = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_VULKAN);
    return true;
}

void VoxSDLWindow::PollEvents() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {

    }
}
