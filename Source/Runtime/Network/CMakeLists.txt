# Source/Runtime/Network/CMakeLists.txt


file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "Private/*.cpp")
add_library(Module.Network STATIC
        ${SOURCES}
)
add_library(VoxCore::Network ALIAS Module.Network)

target_include_directories(Module.Network
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(Module.Network
    PUBLIC
        VoxCore::Core
)
