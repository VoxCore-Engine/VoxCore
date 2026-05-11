
message(STATUS "[VoxCore] Fetching dependencies via CPM...")

CPMAddPackage(
    NAME spdlog
    GITHUB_REPOSITORY gabime/spdlog
    VERSION 1.14.1
    OPTIONS "SPDLOG_BUILD_EXAMPLES OFF" "SPDLOG_BUILD_TESTS OFF"
)

CPMAddPackage(
    NAME glm
    GITHUB_REPOSITORY g-truc/glm
    GIT_TAG 1.0.1
    OPTIONS "GLM_BUILD_TESTS OFF"
)
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)
CPMAddPackage(
    NAME nlohmann_json
    GITHUB_REPOSITORY nlohmann/json
    VERSION 3.11.3
    OPTIONS "JSON_BuildTests OFF"
)

CPMAddPackage(
    NAME FastNoise2
    GITHUB_REPOSITORY Auburn/FastNoise2
    GIT_TAG v0.10.0-alpha
    OPTIONS "FASTNOISE2_TESTS OFF" "FASTNOISE2_NOISETOOL OFF"
)

CPMAddPackage(
    NAME taskflow
    GITHUB_REPOSITORY taskflow/taskflow
    VERSION 3.7.0
    OPTIONS "TF_BUILD_TESTS OFF" "TF_BUILD_EXAMPLES OFF"
)
CPMAddPackage(
        NAME googletest
        GITHUB_REPOSITORY google/googletest
        VERSION 1.15.2
        OPTIONS "INSTALL_GTEST OFF" "BUILD_GMOCK ON"
)
if(VOXCORE_BUILD_CLIENT)
    CPMAddPackage(
        NAME SDL3
        GITHUB_REPOSITORY libsdl-org/SDL
        GIT_TAG release-3.2.14
        OPTIONS
            "SDL_SHARED OFF"
            "SDL_STATIC ON"
            "SDL_TEST_LIBRARY OFF"
    )
    CPMAddPackage(
        NAME VulkanHeaders
        GITHUB_REPOSITORY KhronosGroup/Vulkan-Headers
        GIT_TAG v1.3.296
    )
    CPMAddPackage(
        NAME VulkanHpp
        GITHUB_REPOSITORY KhronosGroup/Vulkan-Hpp
        GIT_TAG v1.4.350
    )
    CPMAddPackage(
        NAME VulkanMemoryAllocator
        GITHUB_REPOSITORY GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
        GIT_TAG v3.1.0
    )
    CPMAddPackage(
            NAME glslang
            GITHUB_REPOSITORY KhronosGroup/glslang
            GIT_TAG 15.1.0
            OPTIONS
            "ENABLE_GLSLANG_BINARIES OFF"
            "ENABLE_HLSL OFF"
            "BUILD_TESTING OFF"
            "BUILD_EXTERNAL OFF"
            "ENABLE_OPT OFF"
    )
    if(VOXCORE_BUILD_EDITOR)
        CPMAddPackage(
            NAME imgui
            GITHUB_REPOSITORY ocornut/imgui
            GIT_TAG docking
            DOWNLOAD_ONLY YES
        )

        CPMAddPackage(
            NAME ImGuizmo
            GITHUB_REPOSITORY CedricGuillemet/ImGuizmo
            GIT_TAG master
            DOWNLOAD_ONLY YES
        )

    endif()
endif()

CPMAddPackage(
        NAME enet
        GITHUB_REPOSITORY lsalzman/enet
        GIT_TAG v1.3.18
)

if(enet_ADDED)
    add_library(enet::enet ALIAS enet)
endif()

message(STATUS "[VoxCore] All dependencies fetched.")
