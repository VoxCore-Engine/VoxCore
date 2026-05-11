file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "Private/*.cpp")

add_library(Module.Core STATIC ${SOURCES})

add_library(VoxCore::Core ALIAS Module.Core)

target_include_directories(Module.Core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(Module.Core
    PUBLIC
        spdlog::spdlog
        glm::glm
        nlohmann_json::nlohmann_json
)

target_compile_features(Module.Core PUBLIC cxx_std_23)

target_compile_definitions(Module.Core PUBLIC
    $<$<PLATFORM_ID:Windows>:VOXCORE_PLATFORM_WINDOWS>
    $<$<PLATFORM_ID:Linux>:VOXCORE_PLATFORM_LINUX>
    $<$<PLATFORM_ID:Darwin>:VOXCORE_PLATFORM_MACOS>
)

voxcore_enable_reflection(
    Module.Core
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "Core"
    "RegisterVoxCoreTypes"
)
