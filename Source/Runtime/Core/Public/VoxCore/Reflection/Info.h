//
// Created by IDKTHIS on 10.05.2026.
//

#pragma once
#include <any>
#include <functional>
#include <typeindex>

#include "Flags.h"
#include "VoxCore/Core/Types/Common.h"
#include "VoxCore/Core/Types/FString.h"
#include "VoxCore/Core/Types/TArray.h"


class UObject;

struct FPropertyInfo {
    FString Name;
    FString TypeName;
    FPropertyFlags Flags = PF_None;
    std::type_index TypeIndex{typeid(void)};
    std::function<TOptional<std::any>(const UObject*)> Getter;
    std::function<bool(UObject*, const std::any&)> Setter;

    [[nodiscard]] bool HasAnyFlags(FPropertyFlags flags) const noexcept {
        return (Flags & flags) != 0;
    }

    template <typename T>
    [[nodiscard]] TOptional<T> GetAs(const UObject* object) const {
        if (!Getter) {
            return std::nullopt;
        }

        auto value = Getter(object);
        if (!value) {
            return std::nullopt;
        }

        if (const auto* typed = std::any_cast<T>(&*value)) {
            return *typed;
        }

        return std::nullopt;
    }
};

struct FFunctionInfo {
    FString Name;
    FString ReturnTypeName;
    TArray<FString> ParameterTypeNames;
    FFunctionFlags Flags = FUNC_None;
    std::function<TOptional<std::any>(UObject*, TSpan<std::any>)> Invoker;

    [[nodiscard]] bool HasAnyFlags(FFunctionFlags flags) const noexcept {
        return (Flags & flags) != 0;
    }

    [[nodiscard]] SIZE_T GetArity() const noexcept { return ParameterTypeNames.size(); }

    template <typename Ret>
        requires (!std::is_void_v<Ret>)
    [[nodiscard]] TOptional<Ret> InvokeAs(UObject* object, TSpan<std::any> args = {}) const {
        if (!Invoker) {
            return std::nullopt;
        }

        auto result = Invoker(object, args);
        if (!result) {
            return std::nullopt;
        }

        if (const auto* typed = std::any_cast<Ret>(&*result)) {
            return *typed;
        }

        return std::nullopt;
    }
};
