#include "VoxEngine/GameFramework/UActor.h"

void UActor::Rename(const FString& newName) {
    Name = newName;
}

const FString& UActor::GetName() const {
    return Name;
}
