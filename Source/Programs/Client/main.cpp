#include <cstdlib>
#include <iostream>

#include "VoxCore/GameFramework/UObject.h"
#include "VoxCore/Reflection/UClass.h"
#include "VoxEngine/FEngineContext.h"
#include "VoxelClient/AActor.h"
#include "VoxelClient/ModuleRegistration.h"

int main(int argc, char** argv) {
    FEngineContext context;
    RegisterVoxelClientTypes(context.GetReflectionRegistry());

    const UClass* actorClass = context.GetReflectionRegistry().FindClass("UActor");
    if (actorClass == nullptr) {
        std::cerr << "UActor was not found in reflection registry\n";
        return EXIT_FAILURE;
    }

    const UClass* localActorClass = context.GetReflectionRegistry().FindClass("AActor");
    if (localActorClass == nullptr) {
        std::cerr << "AActor was not found in reflection registry\n";
        return EXIT_FAILURE;
    }

    TUniquePtr<UObject> actor = actorClass->CreateDefaultObject();
    if (!actor) {
        std::cerr << "UActor default object construction failed\n";
        return EXIT_FAILURE;
    }

    if (!actorClass->SetValue(actor.get(), "Name", FString("VoxelClient"))) {
        std::cerr << "Failed to set Name through reflection\n";
        return EXIT_FAILURE;
    }

    if (!actorClass->SetValue(actor.get(), "Id", int32(7))) {
        std::cerr << "Failed to set Id through reflection\n";
        return EXIT_FAILURE;
    }

    const TOptional<FString> initialName = actorClass->GetValueAs<FString>(actor.get(), "Name");
    const TOptional<int32> initialId = actorClass->GetValueAs<int32>(actor.get(), "Id");
    if (!initialName || !initialId) {
        std::cerr << "Failed to read reflected properties\n";
        return EXIT_FAILURE;
    }

    if (!actorClass->Invoke(actor.get(), "Rename", FString("RenamedFromReflection"))) {
        std::cerr << "Failed to invoke Rename through reflection\n";
        return EXIT_FAILURE;
    }

    const TOptional<FString> renamedName = actorClass->InvokeAs<FString>(actor.get(), "GetName");
    if (!renamedName) {
        std::cerr << "Failed to invoke GetName through reflection\n";
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
    std::cout << "Initial Name: " << *initialName << '\n';
    std::cout << "Initial Id: " << *initialId << '\n';
    std::cout << "Renamed Name: " << *renamedName << '\n';
    std::cout << "Local Actor Name: " << *localActorName << '\n';
    return EXIT_SUCCESS;
}
