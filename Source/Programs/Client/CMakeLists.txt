# Source/Programs/Client/CMakeLists.txt

file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS
    "Private/*.cpp"
)

add_executable(VoxelClient
    main.cpp
    ${CLIENT_SOURCES}
)

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
# На Windows убираем консольное окно в Release
if(WIN32)
    set_target_properties(VoxelClient PROPERTIES
        WIN32_EXECUTABLE $<CONFIG:Release>
    )
endif()

# Копируем папку assets рядом с исполняемым файлом
add_custom_command(TARGET VoxelClient POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:VoxelClient>/assets"
    COMMENT "Copying assets..."
)
