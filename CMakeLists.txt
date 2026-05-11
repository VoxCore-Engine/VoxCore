cmake_minimum_required(VERSION 4.2)
project(VoxCore VERSION 0.0.1
        DESCRIPTION "VoxCore voxel game engine"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CPM_SOURCE_CACHE "$ENV{HOME}/.cache/cpm" CACHE PATH "")

option(VOXCORE_BUILD_CLIENT   "Build the graphical client"  ON)
option(VOXCORE_BUILD_SERVER   "Build the headless dedicated server"                   ON)
option(VOXCORE_BUILD_EDITOR   "Build the level editor (requires VOXEL_BUILD_CLIENT)" OFF)
option(VOXCORE_BUILD_TESTS    "Build unit tests"                                      OFF)
option(VOXCORE_USE_CLANG_TIDY "Enable clang-tidy during compilation" OFF)


if(VOXCORE_USE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    if(CLANG_TIDY_EXE)
        message(STATUS "[VoxCore] clang-tidy enabled: ${CLANG_TIDY_EXE}")
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-config-file=${CMAKE_SOURCE_DIR}/.clang-tidy")
    else()
        message(WARNING "[VoxCore] clang-tidy not found. Install clang-tidy and retry.")
    endif()
endif()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include(cmake/CPM.cmake)
include(cmake/Dependencies.cmake)
include(cmake/VoxReflection.cmake)

add_subdirectory(Tools/VoxHeaderTool)

add_subdirectory(Source/Runtime/Core)
add_subdirectory(Source/Runtime/Engine)
add_subdirectory(Source/Runtime/Network)

if(VOXCORE_BUILD_CLIENT)
    add_subdirectory(Source/Runtime/Platform)
    add_subdirectory(Source/Runtime/Renderer)
endif()

if(VOXCORE_BUILD_EDITOR)
    add_subdirectory(Source/Editor)
endif()

if(VOXCORE_BUILD_CLIENT)
    add_subdirectory(Source/Programs/Client)
endif()

if(VOXCORE_BUILD_SERVER)
    add_subdirectory(Source/Programs/Server)
endif()


message(STATUS "")
message(STATUS "========================================")
message(STATUS "  VoxelCore version: ${PROJECT_VERSION}")
message(STATUS "  Build Client : ${VOXCORE_BUILD_CLIENT}")
message(STATUS "  Build Server : ${VOXCORE_BUILD_SERVER}")
message(STATUS "  Build Editor : ${VOXCORE_BUILD_EDITOR}")
message(STATUS "  Clang-Tidy   : ${VOXCORE_USE_CLANG_TIDY}")
message(STATUS "  CPM Cache    : ${CPM_SOURCE_CACHE}")
message(STATUS "========================================")
message(STATUS "")
