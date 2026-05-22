file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "Private/*.cpp")

add_library(Module.VoxCraft STATIC ${SOURCES})

add_library(VoxProject::VoxCraft ALIAS Module.VoxCraft)

target_include_directories(Module.VoxCraft
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(Module.VoxCraft
    PUBLIC
        VoxCore::Core
        VoxCore::Engine
        VoxCore::Platform
)
