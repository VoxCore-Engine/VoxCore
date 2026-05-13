#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <ostream>
#include <string_view>
#include <variant>

#include "VoxCore/CoreMinimal.h"

enum class ECVarSetSource : uint8 {
    Program = 0,
    Console = 1,
    Config = 2
};

enum class ECVarType : uint8 {
    Integer,
    Float,
    Boolean,
    String
};

enum class ECVarFlags : uint32 {
    None = 0,
    Archive = 1 << 0,
    ReadOnly = 1 << 1,
    Cheat = 1 << 2,
    RuntimeOnly = 1 << 3,
    ConsoleEditable = 1 << 4
};

[[nodiscard]] constexpr ECVarFlags operator|(ECVarFlags lhs, ECVarFlags rhs) noexcept {
    return static_cast<ECVarFlags>(static_cast<uint32>(lhs) | static_cast<uint32>(rhs));
}

[[nodiscard]] constexpr ECVarFlags operator&(ECVarFlags lhs, ECVarFlags rhs) noexcept {
    return static_cast<ECVarFlags>(static_cast<uint32>(lhs) & static_cast<uint32>(rhs));
}

inline ECVarFlags& operator|=(ECVarFlags& lhs, ECVarFlags rhs) noexcept {
    lhs = lhs | rhs;
    return lhs;
}

[[nodiscard]] constexpr bool EnumHasAnyFlags(ECVarFlags value, ECVarFlags flags) noexcept {
    return static_cast<uint32>(value & flags) != 0U;
}

using FCVarValue = std::variant<int32, float32, bool, FString>;
using FCVarCallback = std::function<void(const FCVarValue& oldValue, const FCVarValue& newValue)>;

namespace VoxCVar::Private {
    template <typename TValue>
    [[nodiscard]] FCVarValue MakeValue(TValue&& value) {
        using ValueType = TRemoveCVRef<TValue>;

        if constexpr (std::is_same_v<ValueType, FCVarValue>) {
            return std::forward<TValue>(value);
        } else if constexpr (std::is_same_v<ValueType, FString>) {
            return FCVarValue(std::forward<TValue>(value));
        } else if constexpr (std::is_same_v<ValueType, const char*> || std::is_same_v<ValueType, char*>) {
            return FCVarValue(FString(value));
        } else if constexpr (std::is_convertible_v<ValueType, std::string_view>) {
            return FCVarValue(FString(std::string_view(std::forward<TValue>(value))));
        } else if constexpr (std::is_same_v<ValueType, bool>) {
            return FCVarValue(value);
        } else if constexpr (std::is_integral_v<ValueType>) {
            return FCVarValue(static_cast<int32>(value));
        } else if constexpr (std::is_floating_point_v<ValueType>) {
            return FCVarValue(static_cast<float32>(value));
        } else {
            static_assert(!sizeof(TValue), "Unsupported CVar value type.");
        }
    }

    template <typename TValue>
    [[nodiscard]] TOptional<TValue> TryGet(const FCVarValue& value) {
        if (const TValue* typedValue = std::get_if<TValue>(&value)) {
            return *typedValue;
        }

        return std::nullopt;
    }
}

class FCVarDefinition {
public:
    template <typename TValue>
    FCVarDefinition(FStringView name, TValue&& defaultValue, FStringView description, ECVarFlags flags)
        : FCVarDefinition(
            FString(name),
            VoxCVar::Private::MakeValue(std::forward<TValue>(defaultValue)),
            FString(description),
            flags,
            std::nullopt,
            std::nullopt) {
    }

    template <typename TValue, typename TMinValue, typename TMaxValue>
    FCVarDefinition(
        FStringView name,
        TValue&& defaultValue,
        TMinValue&& minValue,
        TMaxValue&& maxValue,
        FStringView description,
        ECVarFlags flags)
        : FCVarDefinition(
            FString(name),
            VoxCVar::Private::MakeValue(std::forward<TValue>(defaultValue)),
            FString(description),
            flags,
            VoxCVar::Private::MakeValue(std::forward<TMinValue>(minValue)),
            VoxCVar::Private::MakeValue(std::forward<TMaxValue>(maxValue))) {
    }

    [[nodiscard]] const FString& GetName() const noexcept;
    [[nodiscard]] const FString& GetDescription() const noexcept;
    [[nodiscard]] const FCVarValue& GetDefaultValue() const noexcept;
    [[nodiscard]] ECVarFlags GetFlags() const noexcept;
    [[nodiscard]] const TOptional<FCVarValue>& GetMinValue() const noexcept;
    [[nodiscard]] const TOptional<FCVarValue>& GetMaxValue() const noexcept;
    [[nodiscard]] ECVarType GetType() const noexcept;
    [[nodiscard]] FString ToString() const;

private:
    FCVarDefinition(
        FString name,
        FCVarValue defaultValue,
        FString description,
        ECVarFlags flags,
        TOptional<FCVarValue> minValue,
        TOptional<FCVarValue> maxValue);

    static ECVarType ResolveType(const FCVarValue& value);

    FString m_Name;
    FString m_Description;
    FCVarValue m_DefaultValue;
    ECVarFlags m_Flags = ECVarFlags::None;
    TOptional<FCVarValue> m_MinValue;
    TOptional<FCVarValue> m_MaxValue;
    ECVarType m_Type = ECVarType::String;
};

class FCVarRegistry {
public:
    static FCVarRegistry& Get();

    void RegisterDefinition(const FCVarDefinition& definition);

    [[nodiscard]] TArray<const FCVarDefinition*> GetDefinitions() const;
    [[nodiscard]] const FCVarDefinition* FindDefinition(FStringView name) const;
    [[nodiscard]] TArray<FString> GetMatchingNames(FStringView prefix) const;

private:
    mutable std::mutex m_Mutex;
    TArray<const FCVarDefinition*> m_Definitions;
    TMap<FString, const FCVarDefinition*> m_DefinitionsByName;
};

class FCVarManager;

class FCVarSubscription {
public:
    FCVarSubscription() = default;
    FCVarSubscription(const FCVarSubscription&) = delete;
    FCVarSubscription& operator=(const FCVarSubscription&) = delete;

    FCVarSubscription(FCVarSubscription&& other) noexcept;
    FCVarSubscription& operator=(FCVarSubscription&& other) noexcept;

    ~FCVarSubscription();

    void Reset();
    [[nodiscard]] bool IsActive() const noexcept;

private:
    friend class FCVarManager;

    FCVarSubscription(FCVarManager* owner, std::weak_ptr<void> lifetimeToken, FString name, SIZE_T callbackId);

    FCVarManager* m_Owner = nullptr;
    std::weak_ptr<void> m_LifetimeToken;
    FString m_Name;
    SIZE_T m_CallbackId = 0;
};

struct FCVarSnapshot {
    FString Name;
    FString Description;
    FCVarValue Value;
    ECVarFlags Flags = ECVarFlags::None;
    TOptional<FCVarValue> MinValue;
    TOptional<FCVarValue> MaxValue;
    ECVarType Type = ECVarType::String;
};

class FCVarManager {
public:
    FCVarManager();

    [[nodiscard]] bool Contains(FStringView name) const;
    [[nodiscard]] const FCVarDefinition* FindDefinition(FStringView name) const;

    [[nodiscard]] TOptional<FCVarValue> TryGetRaw(FStringView name) const;

    template <typename TValue>
    [[nodiscard]] TOptional<TValue> TryGetValue(FStringView name) const {
        if (const TOptional<FCVarValue> RawValue = TryGetRaw(name); RawValue.has_value()) {
            return VoxCVar::Private::TryGet<TValue>(*RawValue);
        }

        return std::nullopt;
    }

    template <typename TValue>
    [[nodiscard]] TOptional<TValue> TryGetValue(const FCVarDefinition& definition) const {
        return TryGetValue<TValue>(definition.GetName().View());
    }

    template <typename TValue>
    [[nodiscard]] TValue GetValueOr(FStringView name, TValue fallbackValue) const {
        if (const TOptional<TValue> Value = TryGetValue<TValue>(name); Value.has_value()) {
            return *Value;
        }

        return fallbackValue;
    }

    template <typename TValue>
    [[nodiscard]] TValue GetValueOr(const FCVarDefinition& definition, TValue fallbackValue) const {
        return GetValueOr<TValue>(definition.GetName().View(), std::move(fallbackValue));
    }

    bool SetRaw(FStringView name, const FCVarValue& value, ECVarSetSource source = ECVarSetSource::Program);

    template <typename TValue>
    bool SetValue(FStringView name, TValue&& value, ECVarSetSource source = ECVarSetSource::Program) {
        return SetRaw(name, VoxCVar::Private::MakeValue(std::forward<TValue>(value)), source);
    }

    template <typename TValue>
    bool SetValue(const FCVarDefinition& definition, TValue&& value, ECVarSetSource source = ECVarSetSource::Program) {
        return SetValue(definition.GetName().View(), std::forward<TValue>(value), source);
    }

    bool SetFromString(FStringView name, FStringView value, ECVarSetSource source = ECVarSetSource::Console);

    [[nodiscard]] FString GetValueAsString(FStringView name) const;
    [[nodiscard]] FString GetDescription(FStringView name) const;

    FCVarSubscription Subscribe(FStringView name, FCVarCallback callback);
    FCVarSubscription Subscribe(const FCVarDefinition& definition, FCVarCallback callback);

    template <typename TValue, typename TCallback>
    FCVarSubscription SubscribeTyped(FStringView name, TCallback&& callback) {
        return Subscribe(name, [callback = std::forward<TCallback>(callback)](
                                   const FCVarValue& oldValue,
                                   const FCVarValue& newValue) mutable {
            const TValue* oldTypedValue = std::get_if<TValue>(&oldValue);
            const TValue* newTypedValue = std::get_if<TValue>(&newValue);
            if (oldTypedValue == nullptr || newTypedValue == nullptr) {
                return;
            }

            callback(*oldTypedValue, *newTypedValue);
        });
    }

    template <typename TValue, typename TCallback>
    FCVarSubscription SubscribeTyped(const FCVarDefinition& definition, TCallback&& callback) {
        return SubscribeTyped<TValue>(definition.GetName().View(), std::forward<TCallback>(callback));
    }

    void Unsubscribe(FStringView name, SIZE_T callbackId);

    [[nodiscard]] TArray<FString> GetMatchingNames(FStringView prefix) const;
    [[nodiscard]] TArray<FCVarSnapshot> GetAllVariables() const;

    bool SaveToFile(const char* filename, bool onlyArchived = true) const;
    bool LoadFromFile(const char* filename);

private:
    struct FCallbackEntry {
        SIZE_T Id = 0;
        FCVarCallback Callback;
    };

    struct FEntry {
        const FCVarDefinition* Definition = nullptr;
        FCVarValue Value;
        TArray<FCallbackEntry> Callbacks;
    };

    bool ValidateValueLocked(const FEntry& entry, const FCVarValue& value, ECVarSetSource source) const;

    mutable std::mutex m_Mutex;
    TMap<FString, FEntry> m_Entries;
    std::shared_ptr<void> m_LifetimeToken = std::make_shared<int>(0);
    std::atomic<SIZE_T> m_NextCallbackId = 1;
};

#define VOX_PP_CONCAT_INNER(lhs, rhs) lhs##rhs
#define VOX_PP_CONCAT(lhs, rhs) VOX_PP_CONCAT_INNER(lhs, rhs)

#define VOX_DECLARE_CVAR(variableName, cvarName, defaultValue, description, flags) \
    static const FCVarDefinition variableName(cvarName, defaultValue, description, flags)

#define VOX_DECLARE_CVAR_RANGE(variableName, cvarName, defaultValue, minValue, maxValue, description, flags) \
    static const FCVarDefinition variableName(cvarName, defaultValue, minValue, maxValue, description, flags)

#define VOX_DECLARE_CVAR_AUTO(cvarName, defaultValue, description, flags) \
    VOX_DECLARE_CVAR(VOX_PP_CONCAT(GAutoCVar_, __COUNTER__), cvarName, defaultValue, description, flags)

#define VOX_DECLARE_CVAR_RANGE_AUTO(cvarName, defaultValue, minValue, maxValue, description, flags) \
    VOX_DECLARE_CVAR_RANGE(VOX_PP_CONCAT(GAutoCVar_, __COUNTER__), cvarName, defaultValue, minValue, maxValue, description, flags)
