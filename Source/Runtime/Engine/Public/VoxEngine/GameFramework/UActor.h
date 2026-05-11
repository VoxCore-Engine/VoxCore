#pragma once

#include "VoxCore/Core/Types/FString.h"
#include "VoxCore/GameFramework/UObject.h"
#include "VoxCore/Reflection/Flags.h"
#include "VoxEngine/GameFramework/UActor.generated.h"

UCLASS()
class UActor : public UObject {
    GENERATED_BODY()
public:
    UPROPERTY(PF_EditAnywhere | PF_BlueprintReadWrite)
    FString Name;

    UPROPERTY(PF_EditAnywhere | PF_SaveGame)
    int32 Id = 0;

    UFUNCTION(FUNC_None)
    void Rename(const FString& newName);

    UFUNCTION(FUNC_BlueprintPure)
    const FString& GetName() const;
};
