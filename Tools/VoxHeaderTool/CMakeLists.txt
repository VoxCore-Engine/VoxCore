add_executable(VoxHeaderTool main.cpp)

set_target_properties(VoxHeaderTool PROPERTIES
    OUTPUT_NAME "VoxHeaderToolRuntime"
)

target_compile_features(VoxHeaderTool PRIVATE cxx_std_23)

add_executable(VoxHeaderToolLauncher launcher_main.cpp)

set_target_properties(VoxHeaderToolLauncher PROPERTIES
    OUTPUT_NAME "VoxHeaderTool"
)

target_compile_features(VoxHeaderToolLauncher PRIVATE cxx_std_23)

if(MINGW)
    target_link_options(VoxHeaderToolLauncher PRIVATE
        -static-libgcc
        -static-libstdc++
    )
endif()
