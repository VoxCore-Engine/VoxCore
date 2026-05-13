#include "VoxEngine/CVar/FCVar.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>

namespace {
    constexpr std::string_view GCheatToggleCVarName = "sv_cheats";

    [[nodiscard]] FString ValueToString(const FCVarValue& value) {
        return std::visit([](const auto& typedValue) -> FString {
            using ValueType = std::remove_cvref_t<decltype(typedValue)>;

            if constexpr (std::is_same_v<ValueType, bool>) {
                return typedValue ? "true" : "false";
            } else if constexpr (std::is_same_v<ValueType, FString>) {
                return typedValue;
            } else {
                std::ostringstream stream;
                stream << typedValue;
                return FString(stream.str());
            }
        }, value);
    }

    [[nodiscard]] bool StartsWith(std::string_view value, std::string_view prefix) {
        return value.starts_with(prefix);
    }

    void Trim(std::string_view& text) {
        while (!text.empty() && std::isspace(static_cast<unsigned char>(text.front())) != 0) {
            text.remove_prefix(1);
        }

        while (!text.empty() && std::isspace(static_cast<unsigned char>(text.back())) != 0) {
            text.remove_suffix(1);
        }
    }

    [[nodiscard]] bool ParseBool(std::string_view text, bool& outValue) {
        std::string lowered(text);
        std::transform(lowered.begin(), lowered.end(), lowered.begin(), [](unsigned char character) {
            return static_cast<char>(std::tolower(character));
        });

        if (lowered == "true" || lowered == "1" || lowered == "yes" || lowered == "on") {
            outValue = true;
            return true;
        }

        if (lowered == "false" || lowered == "0" || lowered == "no" || lowered == "off") {
            outValue = false;
            return true;
        }

        return false;
    }

    [[nodiscard]] TOptional<FCVarValue> ParseValue(const FCVarValue& currentValue, std::string_view text) {
        try {
            if (std::holds_alternative<int32>(currentValue)) {
                return FCVarValue(static_cast<int32>(std::stoi(std::string(text))));
            }

            if (std::holds_alternative<float32>(currentValue)) {
                return FCVarValue(static_cast<float32>(std::stof(std::string(text))));
            }

            if (std::holds_alternative<bool>(currentValue)) {
                bool parsedValue = false;
                if (!ParseBool(text, parsedValue)) {
                    return std::nullopt;
                }

                return FCVarValue(parsedValue);
            }

            if (std::holds_alternative<FString>(currentValue)) {
                return FCVarValue(FString(text));
            }
        } catch (...) {
            return std::nullopt;
        }

        return std::nullopt;
    }
}

VOX_DECLARE_CVAR(
    GEngineCheatsCVar,
    GCheatToggleCVarName,
    false,
    "Controls whether cheat-protected CVars can be changed.",
    ECVarFlags::RuntimeOnly | ECVarFlags::ConsoleEditable);

FCVarDefinition::FCVarDefinition(
    FString name,
    FCVarValue defaultValue,
    FString description,
    ECVarFlags flags,
    TOptional<FCVarValue> minValue,
    TOptional<FCVarValue> maxValue)
    : m_Name(std::move(name)),
      m_Description(std::move(description)),
      m_DefaultValue(std::move(defaultValue)),
      m_Flags(flags),
      m_MinValue(std::move(minValue)),
      m_MaxValue(std::move(maxValue)),
      m_Type(ResolveType(m_DefaultValue)) {
    FCVarRegistry::Get().RegisterDefinition(*this);
}

const FString& FCVarDefinition::GetName() const noexcept {
    return m_Name;
}

const FString& FCVarDefinition::GetDescription() const noexcept {
    return m_Description;
}

const FCVarValue& FCVarDefinition::GetDefaultValue() const noexcept {
    return m_DefaultValue;
}

ECVarFlags FCVarDefinition::GetFlags() const noexcept {
    return m_Flags;
}

const TOptional<FCVarValue>& FCVarDefinition::GetMinValue() const noexcept {
    return m_MinValue;
}

const TOptional<FCVarValue>& FCVarDefinition::GetMaxValue() const noexcept {
    return m_MaxValue;
}

ECVarType FCVarDefinition::GetType() const noexcept {
    return m_Type;
}

FString FCVarDefinition::ToString() const {
    FString result = m_Name;
    result += " = ";
    result += ValueToString(m_DefaultValue);

    if (m_MinValue.has_value() || m_MaxValue.has_value()) {
        result += " [";
        if (m_MinValue.has_value()) {
            result += ValueToString(*m_MinValue);
        }

        result += "..";

        if (m_MaxValue.has_value()) {
            result += ValueToString(*m_MaxValue);
        }

        result += "]";
    }

    if (!m_Description.IsEmpty()) {
        result += " // ";
        result += m_Description;
    }

    return result;
}

ECVarType FCVarDefinition::ResolveType(const FCVarValue& value) {
    if (std::holds_alternative<int32>(value)) {
        return ECVarType::Integer;
    }

    if (std::holds_alternative<float32>(value)) {
        return ECVarType::Float;
    }

    if (std::holds_alternative<bool>(value)) {
        return ECVarType::Boolean;
    }

    return ECVarType::String;
}

FCVarRegistry& FCVarRegistry::Get() {
    static FCVarRegistry registry;
    return registry;
}

void FCVarRegistry::RegisterDefinition(const FCVarDefinition& definition) {
    std::lock_guard lock(m_Mutex);

    if (m_DefinitionsByName.Contains(definition.GetName())) {
        return;
    }

    m_Definitions.Add(&definition);
    m_DefinitionsByName.Add(definition.GetName(), &definition);
}

TArray<const FCVarDefinition*> FCVarRegistry::GetDefinitions() const {
    std::lock_guard lock(m_Mutex);
    return m_Definitions;
}

const FCVarDefinition* FCVarRegistry::FindDefinition(FStringView name) const {
    std::lock_guard lock(m_Mutex);
    return m_DefinitionsByName.Find(FString(name)) != nullptr ? *m_DefinitionsByName.Find(FString(name)) : nullptr;
}

TArray<FString> FCVarRegistry::GetMatchingNames(FStringView prefix) const {
    std::lock_guard lock(m_Mutex);

    TArray<FString> matches;
    for (const FCVarDefinition* definition : m_Definitions) {
        if (definition != nullptr && StartsWith(definition->GetName().View(), prefix)) {
            matches.Add(definition->GetName());
        }
    }

    return matches;
}

FCVarSubscription::FCVarSubscription(
    FCVarManager* owner,
    std::weak_ptr<void> lifetimeToken,
    FString name,
    SIZE_T callbackId)
    : m_Owner(owner),
      m_LifetimeToken(std::move(lifetimeToken)),
      m_Name(std::move(name)),
      m_CallbackId(callbackId) {
}

FCVarSubscription::FCVarSubscription(FCVarSubscription&& other) noexcept
    : m_Owner(other.m_Owner),
      m_LifetimeToken(std::move(other.m_LifetimeToken)),
      m_Name(std::move(other.m_Name)),
      m_CallbackId(other.m_CallbackId) {
    other.m_Owner = nullptr;
    other.m_CallbackId = 0;
}

FCVarSubscription& FCVarSubscription::operator=(FCVarSubscription&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    Reset();

    m_Owner = other.m_Owner;
    m_LifetimeToken = std::move(other.m_LifetimeToken);
    m_Name = std::move(other.m_Name);
    m_CallbackId = other.m_CallbackId;

    other.m_Owner = nullptr;
    other.m_CallbackId = 0;
    return *this;
}

FCVarSubscription::~FCVarSubscription() {
    Reset();
}

void FCVarSubscription::Reset() {
    if (m_Owner != nullptr && !m_LifetimeToken.expired() && m_CallbackId != 0) {
        m_Owner->Unsubscribe(m_Name.View(), m_CallbackId);
    }

    m_Owner = nullptr;
    m_LifetimeToken.reset();
    m_Name.Reset();
    m_CallbackId = 0;
}

bool FCVarSubscription::IsActive() const noexcept {
    return m_Owner != nullptr && m_CallbackId != 0;
}

FCVarManager::FCVarManager() {
    const TArray<const FCVarDefinition*> definitions = FCVarRegistry::Get().GetDefinitions();

    std::lock_guard lock(m_Mutex);
    for (const FCVarDefinition* definition : definitions) {
        if (definition == nullptr) {
            continue;
        }

        FEntry entry;
        entry.Definition = definition;
        entry.Value = definition->GetDefaultValue();
        m_Entries.Add(definition->GetName(), std::move(entry));
    }
}

bool FCVarManager::Contains(FStringView name) const {
    std::lock_guard lock(m_Mutex);
    return m_Entries.Contains(FString(name));
}

const FCVarDefinition* FCVarManager::FindDefinition(FStringView name) const {
    std::lock_guard lock(m_Mutex);

    if (const FEntry* entry = m_Entries.Find(FString(name)); entry != nullptr) {
        return entry->Definition;
    }

    return nullptr;
}

TOptional<FCVarValue> FCVarManager::TryGetRaw(FStringView name) const {
    std::lock_guard lock(m_Mutex);

    if (const FEntry* entry = m_Entries.Find(FString(name)); entry != nullptr) {
        return entry->Value;
    }

    return std::nullopt;
}

bool FCVarManager::SetRaw(FStringView name, const FCVarValue& value, ECVarSetSource source) {
    TArray<FCallbackEntry> callbacksToInvoke;
    FCVarValue oldValue;

    {
        std::lock_guard lock(m_Mutex);

        FEntry* entry = m_Entries.Find(FString(name));
        if (entry == nullptr) {
            return false;
        }

        if (!ValidateValueLocked(*entry, value, source)) {
            return false;
        }

        if (entry->Value == value) {
            return true;
        }

        oldValue = entry->Value;
        entry->Value = value;
        callbacksToInvoke = entry->Callbacks;
    }

    for (const FCallbackEntry& callbackEntry : callbacksToInvoke) {
        if (callbackEntry.Callback) {
            callbackEntry.Callback(oldValue, value);
        }
    }

    return true;
}

bool FCVarManager::SetFromString(FStringView name, FStringView value, ECVarSetSource source) {
    TOptional<FCVarValue> parsedValue;

    {
        std::lock_guard lock(m_Mutex);

        const FEntry* entry = m_Entries.Find(FString(name));
        if (entry == nullptr) {
            return false;
        }

        parsedValue = ParseValue(entry->Value, value);
    }

    if (!parsedValue.has_value()) {
        return false;
    }

    return SetRaw(name, *parsedValue, source);
}

FString FCVarManager::GetValueAsString(FStringView name) const {
    if (const TOptional<FCVarValue> value = TryGetRaw(name); value.has_value()) {
        return ValueToString(*value);
    }

    return {};
}

FString FCVarManager::GetDescription(FStringView name) const {
    if (const FCVarDefinition* definition = FindDefinition(name); definition != nullptr) {
        return definition->GetDescription();
    }

    return {};
}

FCVarSubscription FCVarManager::Subscribe(FStringView name, FCVarCallback callback) {
    std::lock_guard lock(m_Mutex);

    FEntry* entry = m_Entries.Find(FString(name));
    if (entry == nullptr || !callback) {
        return {};
    }

    const SIZE_T callbackId = m_NextCallbackId.fetch_add(1, std::memory_order_relaxed);
    entry->Callbacks.Add(FCallbackEntry {
        .Id = callbackId,
        .Callback = std::move(callback),
    });

    return FCVarSubscription(this, m_LifetimeToken, FString(name), callbackId);
}

FCVarSubscription FCVarManager::Subscribe(const FCVarDefinition& definition, FCVarCallback callback) {
    return Subscribe(definition.GetName().View(), std::move(callback));
}

void FCVarManager::Unsubscribe(FStringView name, SIZE_T callbackId) {
    std::lock_guard lock(m_Mutex);

    FEntry* entry = m_Entries.Find(FString(name));
    if (entry == nullptr) {
        return;
    }

    for (SIZE_T index = 0; index < entry->Callbacks.Num(); ++index) {
        if (entry->Callbacks[index].Id == callbackId) {
            entry->Callbacks.RemoveAt(index);
            return;
        }
    }
}

TArray<FString> FCVarManager::GetMatchingNames(FStringView prefix) const {
    std::lock_guard lock(m_Mutex);

    TArray<FString> matches;
    for (const auto& [name, entry] : m_Entries) {
        if (StartsWith(name.View(), prefix)) {
            matches.Add(name);
        }
    }

    return matches;
}

TArray<FCVarSnapshot> FCVarManager::GetAllVariables() const {
    std::lock_guard lock(m_Mutex);

    TArray<FCVarSnapshot> snapshots;
    snapshots.Reserve(m_Entries.Num());

    for (const auto& [name, entry] : m_Entries) {
        if (entry.Definition == nullptr) {
            continue;
        }

        snapshots.Add(FCVarSnapshot {
            .Name = name,
            .Description = entry.Definition->GetDescription(),
            .Value = entry.Value,
            .Flags = entry.Definition->GetFlags(),
            .MinValue = entry.Definition->GetMinValue(),
            .MaxValue = entry.Definition->GetMaxValue(),
            .Type = entry.Definition->GetType(),
        });
    }

    return snapshots;
}

bool FCVarManager::SaveToFile(const char* filename, bool onlyArchived) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << "# VoxCore cvar configuration\n\n";

    std::lock_guard lock(m_Mutex);
    for (const auto& [name, entry] : m_Entries) {
        if (entry.Definition == nullptr) {
            continue;
        }

        if (onlyArchived && !EnumHasAnyFlags(entry.Definition->GetFlags(), ECVarFlags::Archive)) {
            continue;
        }

        file << name << ' ';
        if (const FString* stringValue = std::get_if<FString>(&entry.Value); stringValue != nullptr) {
            file << std::quoted(stringValue->Std());
        } else {
            file << ValueToString(entry.Value);
        }

        file << '\n';
    }

    return true;
}

bool FCVarManager::LoadFromFile(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    bool loadedAnything = false;
    std::string line;
    while (std::getline(file, line)) {
        std::string_view lineView(line);
        Trim(lineView);

        if (lineView.empty() || lineView.front() == '#') {
            continue;
        }

        const SIZE_T separatorIndex = lineView.find_first_of(" \t");
        if (separatorIndex == std::string_view::npos) {
            continue;
        }

        std::string_view name = lineView.substr(0, separatorIndex);
        std::string_view value = lineView.substr(separatorIndex + 1);
        Trim(value);

        if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
            value.remove_prefix(1);
            value.remove_suffix(1);
        }

        loadedAnything |= SetFromString(name, value, ECVarSetSource::Config);
    }

    return loadedAnything;
}

bool FCVarManager::ValidateValueLocked(const FEntry& entry, const FCVarValue& value, ECVarSetSource source) const {
    if (entry.Definition == nullptr) {
        return false;
    }

    const ECVarFlags flags = entry.Definition->GetFlags();

    if (EnumHasAnyFlags(flags, ECVarFlags::ReadOnly)) {
        return false;
    }

    if (source == ECVarSetSource::Console && !EnumHasAnyFlags(flags, ECVarFlags::ConsoleEditable)) {
        return false;
    }

    if (source == ECVarSetSource::Config && EnumHasAnyFlags(flags, ECVarFlags::RuntimeOnly)) {
        return false;
    }

    if (entry.Value.index() != value.index()) {
        return false;
    }

    if (EnumHasAnyFlags(flags, ECVarFlags::Cheat) && entry.Definition->GetName().View() != GCheatToggleCVarName) {
        if (const FEntry* cheatsEntry = m_Entries.Find(FString(GCheatToggleCVarName)); cheatsEntry != nullptr) {
            if (const bool* cheatsEnabled = std::get_if<bool>(&cheatsEntry->Value); cheatsEnabled != nullptr && !*cheatsEnabled) {
                return false;
            }
        }
    }

    if (const int32* integerValue = std::get_if<int32>(&value); integerValue != nullptr) {
        if (const TOptional<FCVarValue>& minValue = entry.Definition->GetMinValue();
            minValue.has_value() && std::holds_alternative<int32>(*minValue) && *integerValue < std::get<int32>(*minValue)) {
            return false;
        }

        if (const TOptional<FCVarValue>& maxValue = entry.Definition->GetMaxValue();
            maxValue.has_value() && std::holds_alternative<int32>(*maxValue) && *integerValue > std::get<int32>(*maxValue)) {
            return false;
        }
    }

    if (const float32* floatValue = std::get_if<float32>(&value); floatValue != nullptr) {
        if (const TOptional<FCVarValue>& minValue = entry.Definition->GetMinValue();
            minValue.has_value() && std::holds_alternative<float32>(*minValue) && *floatValue < std::get<float32>(*minValue)) {
            return false;
        }

        if (const TOptional<FCVarValue>& maxValue = entry.Definition->GetMaxValue();
            maxValue.has_value() && std::holds_alternative<float32>(*maxValue) && *floatValue > std::get<float32>(*maxValue)) {
            return false;
        }
    }

    return true;
}
