#include <cstdlib>
#include <iostream>

#include "VoxCore/GameFramework/UObject.h"
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

    if (!localActorClass->SetValue(localActor.get(), "Name", FString("ClientActor"))) {
        std::cerr << "Failed to set AActor.Name through reflection\n";
        return EXIT_FAILURE;
    }

    const TOptional<FString> localActorName = localActorClass->InvokeAs<FString>(localActor.get(), "GetName");
    if (!localActorName) {
        std::cerr << "Failed to invoke AActor.GetName through reflection\n";
        return EXIT_FAILURE;
    }

    std::cout << "Class: " << localActor->GetClass()->ClassName << '\n';
    std::cout << "Local Actor Name: " << *localActorName << '\n';
    return EXIT_SUCCESS;
}
