#include <cstdlib>
#include <iostream>

#include "VoxCore/FApplication.h"
#include "VoxCraft/VoxCraftProject.h"

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    VoxCore::FApplication app;
    if (!app.Initialize()) {
        std::cerr << "Failed to initialize VoxCore application.\n";
        return EXIT_FAILURE;
    }

    VoxCraft::FVoxCraftProject project;
    if (!project.Initialize(*app.GetEngineContext())) {
        std::cerr << "Failed to initialize VoxCraft project.\n";
        app.Shutdown();
        return EXIT_FAILURE;
    }

    app.Run();

    project.Shutdown(*app.GetEngineContext());
    app.Shutdown();
    return EXIT_SUCCESS;
}
