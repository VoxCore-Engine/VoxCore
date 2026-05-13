#pragma once

#include "VoxCore/CoreMinimal.h"
#include "VoxCore/GameFramework/UObject.h"

#include "AActor.generated.h"

UCLASS()
class AActor : public UObject {
    GENERATED_BODY()
public:
    UPROPERTY(PF_EditAnywhere | PF_BlueprintReadWrite)
    FTransform Transform;

    UFUNCTION(FUNC_BlueprintPure)
    [[nodiscard]] FTransform GetActorTransform() const {
        return Transform;
    }

    UFUNCTION(FUNC_None)
    void SetActorTransform(const FTransform& inTransform) {
        Transform = inTransform;
    }

    UFUNCTION(FUNC_BlueprintPure)
    [[nodiscard]] FVector GetActorLocation() const {
        return Transform.Translation;
    }

    UFUNCTION(FUNC_None)
    void SetActorLocation(const FVector& inLocation) {
        Transform.Translation = inLocation;
    }
};
