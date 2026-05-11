# Source/Runtime/Platform/CMakeLists.txt
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "Private/*.cpp")

add_library(Module.Platform STATIC
    ${SOURCES}
)

add_library(VoxCore::Platform ALIAS Module.Platform)

target_include_directories(Module.Platform
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(Module.Platform
    PUBLIC
        VoxCore::Core
        SDL3::SDL3-static
)
