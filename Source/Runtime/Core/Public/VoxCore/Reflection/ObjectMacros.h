#pragma once

#define UCLASS(...)
#define UPROPERTY(...)
#define UFUNCTION(...)

#define VOXCORE_PP_CAT(lhs, rhs) VOXCORE_PP_CAT_IMPL(lhs, rhs)
#define VOXCORE_PP_CAT_IMPL(lhs, rhs) lhs##rhs
#define VOXCORE_PP_CAT3(first, second, third) VOXCORE_PP_CAT(VOXCORE_PP_CAT(first, second), third)
#define VOXCORE_PP_CAT4(first, second, third, fourth) VOXCORE_PP_CAT(VOXCORE_PP_CAT3(first, second, third), fourth)

#define GENERATED_BODY() VOXCORE_PP_CAT4(VOXCORE_CURRENT_FILE_ID, _, __LINE__, _GENERATED_BODY)
