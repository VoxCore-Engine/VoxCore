add_library(Module.Editor STATIC
    Private/VoxCore.cpp
)

add_library(VoxCore::Editor ALIAS Module.Editor)

target_include_directories(Module.Editor
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(Module.Editor
    PUBLIC
        VoxCore::Core
        VoxCore::Engine
)
