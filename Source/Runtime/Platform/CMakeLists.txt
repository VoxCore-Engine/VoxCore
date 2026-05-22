# Source/Runtime/Platform/CMakeLists.txt
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "Private/*.cpp")

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
add_subdirectory(ThirdParty/SDL EXCLUDE_FROM_ALL)

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
        VoxCore::Engine
        SDL3::SDL3-static
)
