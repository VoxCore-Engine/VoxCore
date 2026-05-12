
file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS
    "Private/*.cpp"
)

add_executable(VoxelClient
    main.cpp
    ${CLIENT_SOURCES}
)

set_target_properties(VoxelClient PROPERTIES
    OUTPUT_NAME "VoxClientRuntime"
)

add_executable(VoxClientLauncher launcher_main.cpp)

set_target_properties(VoxClientLauncher PROPERTIES
    OUTPUT_NAME "VoxClient"
)

if(MINGW)
    target_link_options(VoxClientLauncher PRIVATE
        -static-libgcc
        -static-libstdc++
    )
endif()

target_include_directories(VoxelClient PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Public
    ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(VoxelClient PRIVATE
    VoxCore::Core
    VoxCore::Engine
    VoxCore::Network
    VoxCore::Platform
    VoxCore::Renderer
)

voxcore_enable_reflection(
    VoxelClient
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "Client"
    "RegisterVoxelClientTypes"
)

if(WIN32)
    set_target_properties(VoxelClient VoxClientLauncher PROPERTIES
        WIN32_EXECUTABLE $<CONFIG:Release>
    )
endif()

set(VOXCORE_CLIENT_DIST_DIR "${CMAKE_BINARY_DIR}/dist/$<CONFIG>")

if(MINGW)
    get_filename_component(VOXCORE_MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    set(VOXCORE_MINGW_RUNTIME_COPY_COMMAND
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${VOXCORE_MINGW_BIN_DIR}/libstdc++-6.dll"
            "${VOXCORE_MINGW_BIN_DIR}/libgcc_s_seh-1.dll"
            "${VOXCORE_MINGW_BIN_DIR}/libwinpthread-1.dll"
            "${VOXCORE_CLIENT_DIST_DIR}/bin"
    )
endif()

add_dependencies(VoxelClient
    VoxHeaderTool
    VoxClientLauncher
    VoxHeaderToolLauncher
)

add_custom_command(TARGET VoxelClient POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${VOXCORE_CLIENT_DIST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${VOXCORE_CLIENT_DIST_DIR}/Tools"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${VOXCORE_CLIENT_DIST_DIR}/Content"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${VOXCORE_CLIENT_DIST_DIR}/bin"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:VoxClientLauncher>"
        "${VOXCORE_CLIENT_DIST_DIR}/VoxClient.exe"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:VoxelClient>"
        "${VOXCORE_CLIENT_DIST_DIR}/bin/VoxClientRuntime.exe"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:VoxHeaderToolLauncher>"
        "${VOXCORE_CLIENT_DIST_DIR}/Tools/VoxHeaderTool.exe"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:VoxHeaderTool>"
        "${VOXCORE_CLIENT_DIST_DIR}/bin/VoxHeaderToolRuntime.exe"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Content"
        "${VOXCORE_CLIENT_DIST_DIR}/Content"
    ${VOXCORE_MINGW_RUNTIME_COPY_COMMAND}
    COMMAND_EXPAND_LISTS
    COMMENT "Staging VoxClient distribution layout"
)
