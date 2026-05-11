#pragma once

#include "VoxCore/Core/Types/FString.h"
#include "VoxCore/GameFramework/UObject.h"
#include "VoxCore/Reflection/Flags.h"
#include "VoxelClient/AActor.generated.h"

UCLASS()
class AActor : public UObject {
    GENERATED_BODY()
public:
    UPROPERTY(PF_EditAnywhere | PF_BlueprintReadWrite)
    FString Name = "DefaultActor";

    UFUNCTION(FUNC_BlueprintPure)
    FString GetName() const;
};
