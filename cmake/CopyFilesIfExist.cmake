if(NOT DEFINED DESTINATION)
    message(FATAL_ERROR "DESTINATION is required")
endif()

file(MAKE_DIRECTORY "${DESTINATION}")

if(NOT DEFINED FILES OR FILES STREQUAL "")
    return()
endif()

string(REPLACE "|" ";" FILES_LIST "${FILES}")

foreach(FILE_PATH IN LISTS FILES_LIST)
    if(EXISTS "${FILE_PATH}")
        execute_process(
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${FILE_PATH}" "${DESTINATION}"
            COMMAND_ERROR_IS_FATAL ANY
        )
    endif()
endforeach()
