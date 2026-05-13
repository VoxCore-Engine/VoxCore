#pragma once

#include <any>
#include <functional>
#include <type_traits>

#include "VoxCore/CoreMinimal.h"
#include "VoxCore/Reflection/Info.h"

class UObject;

class UClass {
public:
    using Factory = std::function<TUniquePtr<UObject>()>;

    FString ClassName;
    UClass* SuperClass = nullptr;
    TArray<FPropertyInfo> Properties;
    TArray<FFunctionInfo> Functions;
    Factory Construct;

    [[nodiscard]] const FPropertyInfo* FindProperty(FStringView name, bool searchSuper = true) const;
    [[nodiscard]] const FFunctionInfo* FindFunction(FStringView name, bool searchSuper = true) const;
    [[nodiscard]] bool IsA(const UClass* other) const noexcept;

    template <typename Visitor>
    void ForEachProperty(Visitor&& visitor, bool includeSuper = true) const {
        if (includeSuper && SuperClass != nullptr) {
            SuperClass->ForEachProperty(visitor, true);
        }

        for (const auto& property : Properties) {
            visitor(property);
        }
    }

    template <typename Visitor>
    void ForEachFunction(Visitor&& visitor, bool includeSuper = true) const {
        if (includeSuper && SuperClass != nullptr) {
            SuperClass->ForEachFunction(visitor, true);
        }

        for (const auto& function : Functions) {
            visitor(function);
        }
    }

    [[nodiscard]] TArray<const FPropertyInfo*> GetAllProperties() const;
    [[nodiscard]] TArray<const FFunctionInfo*> GetAllFunctions() const;
    [[nodiscard]] TOptional<std::any> GetValue(const UObject* object, FStringView name) const;

    template <typename T>
    [[nodiscard]] TOptional<T> GetValueAs(const UObject* object, FStringView name) const {
        if (const auto* property = FindProperty(name)) {
            return property->GetAs<T>(object);
        }

        return std::nullopt;
    }

    [[nodiscard]] bool SetValue(UObject* object, FStringView name, const std::any& value) const;

    template <typename T>
        requires (!std::is_same_v<std::remove_cvref_t<T>, std::any>)
    [[nodiscard]] bool SetValue(UObject* object, FStringView name, T&& value) const {
        return SetValue(object, name, std::any(std::forward<T>(value)));
    }

    [[nodiscard]] TOptional<std::any> Invoke(UObject* object, FStringView name, TSpan<std::any> args = {}) const;

    template <typename... Args>
    [[nodiscard]] TOptional<std::any> Invoke(UObject* object, FStringView name, Args&&... args) const {
        TArray<std::any> packedArgs;
        packedArgs.reserve(sizeof...(Args));
        (packedArgs.emplace_back(std::forward<Args>(args)), ...);
        return Invoke(object, name, TSpan<std::any>(packedArgs));
    }

    template <typename Ret, typename... Args>
        requires (!std::is_void_v<Ret>)
    [[nodiscard]] TOptional<Ret> InvokeAs(UObject* object, FStringView name, Args&&... args) const {
        auto result = Invoke(object, name, std::forward<Args>(args)...);
        if (!result) {
            return std::nullopt;
        }

        if (const auto* typed = std::any_cast<Ret>(&*result)) {
            return *typed;
        }

        return std::nullopt;
    }

    [[nodiscard]] TUniquePtr<UObject> CreateDefaultObject() const;
};
