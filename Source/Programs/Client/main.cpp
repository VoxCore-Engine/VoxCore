#include <cstdlib>
#include <iostream>

#include "VoxCore/CoreMinimal.h"
#include "VoxCore/GameFramework/UObject.h"
#include "VoxCore/Platform/Window/VoxSDLWindow.h"
#include "VoxCore/Reflection/UClass.h"
#include "VoxEngine/FEngineContext.h"

#include "VoxelClient/ModuleRegistration.h"

int main(int argc, char** argv) {
    FEngineContext context;
    RegisterVoxelClientTypes(context.GetReflectionRegistry());

    const UClass* localActorClass = context.GetReflectionRegistry().FindClass("AActor");
    if (localActorClass == nullptr) {
        std::cerr << "AActor was not found in reflection registry\n";
        return EXIT_FAILURE;
    }

    TUniquePtr<UObject> localActor = localActorClass->CreateDefaultObject();
    if (!localActor) {
        std::cerr << "AActor default object construction failed\n";
        return EXIT_FAILURE;
    }



    std::cout << "Class: " << localActor->GetClass()->ClassName << '\n';


    VoxCore::Platform::Window::VoxSDLWindow testWindow = VoxCore::Platform::Window::VoxSDLWindow();
    testWindow.Create(1920, 1080, "Test Window");
    while (1) {
        testWindow.PollEvents();
    }
    return EXIT_SUCCESS;
}
