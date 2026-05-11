#include "VoxEngine/Reflection/FReflectionRegistry.h"

#include "VoxCore/Reflection/UClass.h"

void FReflectionRegistry::RegisterClass(UClass* cls) {
    if (cls == nullptr) {
        return;
    }

    if (RegisteredClassesByName.Contains(cls->ClassName)) {
        return;
    }

    RegisteredClasses.Add(cls);
    RegisteredClassesByName.Add(cls->ClassName, cls);
}

const UClass* FReflectionRegistry::FindClass(FStringView name) const {
    if (const auto* cls = RegisteredClassesByName.Find(FString(name))) {
        return *cls;
    }

    return nullptr;
}

TSpan<UClass* const> FReflectionRegistry::GetRegisteredClasses() const noexcept {
    return {RegisteredClasses.data(), RegisteredClasses.size()};
}

void FReflectionRegistry::Reset() {
    RegisteredClasses.Reset();
    RegisteredClassesByName = TMap<FString, UClass*>{};
}
