//
// Created by IDKTHIS on 10.05.2026.
//

#pragma once
#include "VoxCore/Core/Types/Common.h"

using FFunctionFlags = uint32;
using FPropertyFlags = uint32;

inline constexpr FFunctionFlags FUNC_None = 0;
inline constexpr FFunctionFlags FUNC_BlueprintCallable = 1u << 0;
inline constexpr FFunctionFlags FUNC_BlueprintPure = 1u << 1;
inline constexpr FFunctionFlags FUNC_Exec = 1u << 2;
inline constexpr FFunctionFlags FUNC_Server = 1u << 3;
inline constexpr FFunctionFlags FUNC_Client = 1u << 4;
inline constexpr FFunctionFlags FUNC_NetMulticast = 1u << 5;
inline constexpr FFunctionFlags FUNC_CallInEditor = 1u << 6;
inline constexpr FFunctionFlags FUNC_Reliable = 1u << 7;


inline constexpr FPropertyFlags PF_None = 0;
inline constexpr FPropertyFlags PF_EditAnywhere = 1u << 0;
inline constexpr FPropertyFlags PF_EditDefaultsOnly = 1u << 1;
inline constexpr FPropertyFlags PF_EditInstanceOnly = 1u << 2;
inline constexpr FPropertyFlags PF_BlueprintReadWrite = 1u << 3;
inline constexpr FPropertyFlags PF_BlueprintReadOnly = 1u << 4;
inline constexpr FPropertyFlags PF_Transient = 1u << 5;
inline constexpr FPropertyFlags PF_SaveGame = 1u << 6;
inline constexpr FPropertyFlags PF_VisibleAnywhere = 1u << 7;
inline constexpr FPropertyFlags PF_VisibleDefaultsOnly = 1u << 8;
inline constexpr FPropertyFlags PF_Replicated = 1u << 9;
inline constexpr FPropertyFlags PF_Config = 1u << 10;
inline constexpr FPropertyFlags PF_NoClear = 1u << 11;