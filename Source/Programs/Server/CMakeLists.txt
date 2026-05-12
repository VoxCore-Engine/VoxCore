
add_executable(VoxelServer main.cpp)

target_link_libraries(VoxelServer PRIVATE
    VoxCore::Core
    VoxCore::Engine
    VoxCore::Network
)

target_compile_definitions(VoxelServer PRIVATE VE_HEADLESS_SERVER)
