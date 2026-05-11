
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "Private/*.cpp")

add_library(Module.Engine STATIC ${SOURCES})

add_library(VoxCore::Engine ALIAS Module.Engine)

target_include_directories(Module.Engine
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(Module.Engine
    PUBLIC
        VoxCore::Core
        Taskflow
        FastNoise2
)

voxcore_enable_reflection(
    Module.Engine
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "Engine"
    "RegisterVoxEngineTypes"
)
