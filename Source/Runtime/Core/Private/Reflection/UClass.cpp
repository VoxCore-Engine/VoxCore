#include "VoxCore/Reflection/UClass.h"

#include "VoxCore/GameFramework/UObject.h"

const FPropertyInfo* UClass::FindProperty(FStringView name, bool searchSuper) const {
    for (const auto& property : Properties) {
        if (property.Name == name) {
            return &property;
        }
    }

    if (searchSuper && SuperClass != nullptr) {
        return SuperClass->FindProperty(name, true);
    }

    return nullptr;
}

const FFunctionInfo* UClass::FindFunction(FStringView name, bool searchSuper) const {
    for (const auto& function : Functions) {
        if (function.Name == name) {
            return &function;
        }
    }

    if (searchSuper && SuperClass != nullptr) {
        return SuperClass->FindFunction(name, true);
    }

    return nullptr;
}

bool UClass::IsA(const UClass* other) const noexcept {
    for (const UClass* current = this; current != nullptr; current = current->SuperClass) {
        if (current == other) {
            return true;
        }
    }

    return false;
}

TArray<const FPropertyInfo*> UClass::GetAllProperties() const {
    TArray<const FPropertyInfo*> properties;
    ForEachProperty([&properties](const FPropertyInfo& property) {
        properties.push_back(&property);
    });
    return properties;
}

TArray<const FFunctionInfo*> UClass::GetAllFunctions() const {
    TArray<const FFunctionInfo*> functions;
    ForEachFunction([&functions](const FFunctionInfo& function) {
        functions.push_back(&function);
    });
    return functions;
}

TOptional<std::any> UClass::GetValue(const UObject* object, FStringView name) const {
    if (const auto* property = FindProperty(name)) {
        return property->Getter(object);
    }

    return std::nullopt;
}

bool UClass::SetValue(UObject* object, FStringView name, const std::any& value) const {
    if (const auto* property = FindProperty(name)) {
        return property->Setter(object, value);
    }

    return false;
}

TOptional<std::any> UClass::Invoke(UObject* object, FStringView name, TSpan<std::any> args) const {
    if (const auto* function = FindFunction(name)) {
        return function->Invoker(object, args);
    }

    return std::nullopt;
}

TUniquePtr<UObject> UClass::CreateDefaultObject() const {
    if (!Construct) {
        return {};
    }

    return Construct();
}
