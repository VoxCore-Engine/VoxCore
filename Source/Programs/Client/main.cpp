#include <cstdlib>
#include <iostream>

#include "VoxCore/CoreMinimal.h"
#include "VoxCore/GameFramework/UObject.h"
#include "VoxCore/Platform/Window/VoxSDLWindow.h"
#include "VoxCore/Reflection/UClass.h"
#include "VoxEngine/FEngineContext.h"

#include "VoxelClient/ModuleRegistration.h"

VOX_DECLARE_CVAR_RANGE(
    GClientTestCVar,
    "client.test",
    1.1f,
    0.0f,
    10.0f,
    "Small test CVar that demonstrates declaration, reads, writes and subscriptions.",
    ECVarFlags::Archive | ECVarFlags::ConsoleEditable);

int main(int argc, char** argv) {
    FEngineContext context;
    RegisterVoxelClientTypes(context.GetReflectionRegistry());
    FCVarManager& cvars = context.GetCVarManager();

    FCVarSubscription testSubscription = cvars.SubscribeTyped<float32>(
        GClientTestCVar,
        [](float32 oldValue, float32 newValue) {
            std::cout << "[CVar] client.test changed: " << oldValue << " -> " << newValue << '\n';
        });

    std::cout << "[CVar] initial " << GClientTestCVar.GetName() << " = "
              << cvars.GetValueOr<float32>(GClientTestCVar, 0.0f) << '\n';

    cvars.SetValue(GClientTestCVar, 2.5f);
    cvars.SetFromString(GClientTestCVar.GetName().View(), "3.75", ECVarSetSource::Console);

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
    for (const auto& cvar : cvars.GetAllVariables()) {
        std::visit([cvar](auto&& Arg) {
            std::cout << cvar.Name << ": " << Arg << '\n';
        }, cvar.Value);
    }


    VoxCore::Platform::Window::VoxSDLWindow testWindow = VoxCore::Platform::Window::VoxSDLWindow();
    testWindow.Create(1920, 1080, "Test Window");

    return EXIT_SUCCESS;
}
