#pragma once

#include <any>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <utility>

#include "VoxCore/Core/Types/FString.h"
#include "VoxCore/Core/Types/TArray.h"
#include "VoxCore/GameFramework/UObject.h"
#include "VoxCore/Memory/Pointers.h"
#include "VoxCore/Reflection/Flags.h"
#include "VoxCore/Reflection/Info.h"

namespace voxcore::reflection::detail {
    template <typename T>
    using TDecay = std::remove_cv_t<std::remove_reference_t<T>>;

    template <typename T>
    [[nodiscard]] FString GetTypeName() {
        using Type = TDecay<T>;

        if constexpr (std::is_same_v<Type, bool>) {
            return "bool";
        } else if constexpr (std::is_void_v<Type>) {
            return "void";
        } else if constexpr (std::is_same_v<Type, int8>) {
            return "int8";
        } else if constexpr (std::is_same_v<Type, int16>) {
            return "int16";
        } else if constexpr (std::is_same_v<Type, int32>) {
            return "int32";
        } else if constexpr (std::is_same_v<Type, int64>) {
            return "int64";
        } else if constexpr (std::is_same_v<Type, uint8>) {
            return "uint8";
        } else if constexpr (std::is_same_v<Type, uint16>) {
            return "uint16";
        } else if constexpr (std::is_same_v<Type, uint32>) {
            return "uint32";
        } else if constexpr (std::is_same_v<Type, uint64>) {
            return "uint64";
        } else if constexpr (std::is_same_v<Type, float>) {
            return "float";
        } else if constexpr (std::is_same_v<Type, double>) {
            return "double";
        } else if constexpr (std::is_same_v<Type, FString>) {
            return "FString";
        } else if constexpr (std::is_same_v<Type, std::string>) {
            return "std::string";
        } else {
            return typeid(Type).name();
        }
    }

    template <auto Member>
    struct TMemberObjectTraits;

    template <typename ClassType, typename ValueType, ValueType ClassType::*Member>
    struct TMemberObjectTraits<Member> {
        using Class = ClassType;
        using Value = ValueType;
    };

    template <typename T>
    [[nodiscard]] TOptional<TDecay<T>> CastAnyValue(const std::any& value) {
        using ValueType = TDecay<T>;

        if (const auto* typed = std::any_cast<ValueType>(&value)) {
            return *typed;
        }

        return std::nullopt;
    }

    template <auto Member>
    [[nodiscard]] FPropertyInfo MakePropertyInfo(const char* name, FPropertyFlags flags) {
        using Traits = TMemberObjectTraits<Member>;
        using ClassType = typename Traits::Class;
        using ValueType = typename Traits::Value;
        using StoredType = TDecay<ValueType>;

        FPropertyInfo info;
        info.Name = name;
        info.TypeName = GetTypeName<ValueType>();
        info.Flags = flags;
        info.TypeIndex = std::type_index(typeid(StoredType));
        info.Getter = [](const UObject* object) -> TOptional<std::any> {
            const auto* typedObject = dynamic_cast<const ClassType*>(object);
            if (typedObject == nullptr) {
                return std::nullopt;
            }

            if constexpr (std::copy_constructible<StoredType>) {
                return std::any(typedObject->*Member);
            } else {
                return std::nullopt;
            }
        };
        info.Setter = [](UObject* object, const std::any& value) -> bool {
            if constexpr (std::is_const_v<std::remove_reference_t<ValueType>>) {
                return false;
            } else if constexpr (!std::is_assignable_v<ValueType&, StoredType>) {
                return false;
            } else {
                auto* typedObject = dynamic_cast<ClassType*>(object);
                if (typedObject == nullptr) {
                    return false;
                }

                const auto typedValue = CastAnyValue<ValueType>(value);
                if (!typedValue) {
                    return false;
                }

                typedObject->*Member = *typedValue;
                return true;
            }
        };
        return info;
    }

    template <typename>
    struct TMemberFunctionTraits;

    template <typename ClassType, typename ReturnType, typename... Args>
    struct TMemberFunctionTraits<ReturnType (ClassType::*)(Args...)> {
        using Class = ClassType;
        using Return = ReturnType;
        using Tuple = std::tuple<Args...>;

        static constexpr SIZE_T Arity = sizeof...(Args);
        static constexpr bool IsConst = false;
    };

    template <typename ClassType, typename ReturnType, typename... Args>
    struct TMemberFunctionTraits<ReturnType (ClassType::*)(Args...) const> {
        using Class = ClassType;
        using Return = ReturnType;
        using Tuple = std::tuple<Args...>;

        static constexpr SIZE_T Arity = sizeof...(Args);
        static constexpr bool IsConst = true;
    };

    template <typename Tuple, SIZE_T... Indices>
    [[nodiscard]] TArray<FString> MakeParameterTypeNames(std::index_sequence<Indices...>) {
        return {GetTypeName<std::tuple_element_t<Indices, Tuple>>()...};
    }

    template <typename ReturnType, typename ObjectType, auto Method, typename Tuple, SIZE_T... Indices>
    [[nodiscard]] TOptional<std::any> InvokeMethod(ObjectType& object, TSpan<std::any> args, std::index_sequence<Indices...>) {
        static_assert(((!std::is_lvalue_reference_v<std::tuple_element_t<Indices, Tuple>> ||
                        std::is_const_v<std::remove_reference_t<std::tuple_element_t<Indices, Tuple>>>) && ...));

        auto converted = std::tuple<TOptional<TDecay<std::tuple_element_t<Indices, Tuple>>>...>{
            CastAnyValue<std::tuple_element_t<Indices, Tuple>>(args[Indices])...
        };

        const bool allValid = (... && static_cast<bool>(std::get<Indices>(converted)));
        if (!allValid) {
            return std::nullopt;
        }

        if constexpr (std::is_void_v<ReturnType>) {
            (object.*Method)(*std::get<Indices>(converted)...);
            return std::any();
        } else {
            using StoredReturnType = TDecay<ReturnType>;
            return std::any(StoredReturnType((object.*Method)(*std::get<Indices>(converted)...)));
        }
    }

    template <auto Method>
    [[nodiscard]] FFunctionInfo MakeFunctionInfo(const char* name, FFunctionFlags flags) {
        using Traits = TMemberFunctionTraits<decltype(Method)>;
        using ClassType = typename Traits::Class;
        using ReturnType = typename Traits::Return;
        using TupleType = typename Traits::Tuple;

        FFunctionInfo info;
        info.Name = name;
        info.ReturnTypeName = GetTypeName<ReturnType>();
        info.ParameterTypeNames = MakeParameterTypeNames<TupleType>(std::make_index_sequence<Traits::Arity>{});
        info.Flags = flags;
        info.Invoker = [](UObject* object, TSpan<std::any> args) -> TOptional<std::any> {
            if (args.size() != Traits::Arity) {
                return std::nullopt;
            }

            if constexpr (Traits::IsConst) {
                const auto* typedObject = dynamic_cast<const ClassType*>(object);
                if (typedObject == nullptr) {
                    return std::nullopt;
                }

                return InvokeMethod<ReturnType, const ClassType, Method, TupleType>(
                    *typedObject,
                    args,
                    std::make_index_sequence<Traits::Arity>{});
            } else {
                auto* typedObject = dynamic_cast<ClassType*>(object);
                if (typedObject == nullptr) {
                    return std::nullopt;
                }

                return InvokeMethod<ReturnType, ClassType, Method, TupleType>(
                    *typedObject,
                    args,
                    std::make_index_sequence<Traits::Arity>{});
            }
        };
        return info;
    }
}
