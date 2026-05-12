function(voxcore_collect_reflection_headers OUTPUT_VARIABLE ROOT_DIR)
    set(headers)

    if(EXISTS "${ROOT_DIR}/Public")
        file(GLOB_RECURSE public_headers CONFIGURE_DEPENDS
            "${ROOT_DIR}/Public/*.h"
            "${ROOT_DIR}/Public/*.hpp"
        )
        list(APPEND headers ${public_headers})
    endif()

    if(EXISTS "${ROOT_DIR}/Private")
        file(GLOB_RECURSE private_headers CONFIGURE_DEPENDS
            "${ROOT_DIR}/Private/*.h"
            "${ROOT_DIR}/Private/*.hpp"
        )
        list(APPEND headers ${private_headers})
    endif()

    set(${OUTPUT_VARIABLE} ${headers} PARENT_SCOPE)
endfunction()

function(voxcore_append_generated_headers OUTPUT_VARIABLE ROOT_DIR GENERATED_ROOT)
    set(generated_headers)

    foreach(header IN LISTS ARGN)
        get_filename_component(base_name "${header}" NAME_WE)

        if(header MATCHES "/Public/")
            list(APPEND generated_headers "${GENERATED_ROOT}/Public/${base_name}.generated.h")
        elseif(header MATCHES "/Private/")
            list(APPEND generated_headers "${GENERATED_ROOT}/Private/${base_name}.generated.h")
        endif()
    endforeach()

    set(${OUTPUT_VARIABLE} ${generated_headers} PARENT_SCOPE)
endfunction()


function(voxcore_enable_reflection TARGET ROOT_DIR MODULE_NAME REGISTRATION_FUNCTION)
    voxcore_collect_reflection_headers(reflection_headers "${ROOT_DIR}")

    set(generated_root "${CMAKE_BINARY_DIR}/Generated/${MODULE_NAME}")
    set(generated_public_root "${generated_root}/Public")
    set(generated_private_root "${generated_root}/Private")
    set(generated_source "${generated_private_root}/${MODULE_NAME}.Reflection.gen.cpp")

    voxcore_append_generated_headers(generated_headers "${ROOT_DIR}" "${generated_root}" ${reflection_headers})

    add_custom_command(
        OUTPUT ${generated_headers} "${generated_source}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${generated_public_root}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${generated_private_root}"
        COMMAND $<TARGET_FILE:VoxHeaderTool>
            --module-name "${MODULE_NAME}"
            --module-root "${ROOT_DIR}"
            --public-root "${ROOT_DIR}/Public"
            --private-root "${ROOT_DIR}/Private"
            --output-public-root "${generated_public_root}"
            --output-private-root "${generated_private_root}"
            --registration-function "${REGISTRATION_FUNCTION}"
        DEPENDS VoxHeaderTool ${reflection_headers}
        VERBATIM
        COMMENT "Generating reflection for ${MODULE_NAME}"
    )

    target_include_directories(${TARGET}
        PUBLIC
            "${generated_public_root}"
        PRIVATE
            "${generated_private_root}"
    )

    target_sources(${TARGET} PRIVATE "${generated_source}")

    set_source_files_properties(${generated_headers} PROPERTIES GENERATED TRUE HEADER_FILE_ONLY TRUE)
    set_source_files_properties("${generated_source}" PROPERTIES GENERATED TRUE)
endfunction()
