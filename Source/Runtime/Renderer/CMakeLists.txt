# Source/Runtime/Renderer/CMakeLists.txt
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "Private/*.cpp")

add_library(Module.Renderer STATIC
        ${SOURCES}
)

add_library(VoxCore::Renderer ALIAS Module.Renderer)

target_include_directories(Module.Renderer
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

find_package(Vulkan REQUIRED)

target_link_libraries(Module.Renderer
    PUBLIC
        VoxCore::Core
        VoxCore::Platform
        Vulkan::Vulkan
        VulkanMemoryAllocator
        glslang
)

target_compile_definitions(Module.Renderer PUBLIC VULKAN_HPP_NO_EXCEPTIONS)
