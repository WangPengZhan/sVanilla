set(TARGET_EXECUTABLE_PATH "${EXECUTABLE_PATH}")

function(kill_process_by_path executable_path)
    if(WIN32)
        string(REPLACE "/" "\\\\" executable_path "${executable_path}.exe")

        execute_process(
            COMMAND wmic process where "ExecutablePath='${executable_path}'" call terminate
            RESULT_VARIABLE result
            ERROR_QUIET
        )
    elseif(APPLE OR UNIX)
        # execute_process(
        #     COMMAND pkill -f ${executable_path}
        #     RESULT_VARIABLE result
        #     ERROR_QUIET
        # )
    endif()

    if(result EQUAL 0)
        message(STATUS "Successfully terminated process at path: ${executable_path}")
    else()
        message(WARNING "Failed to terminate process at path: ${executable_path} or process not found.")
    endif()
endfunction()

kill_process_by_path("${TARGET_EXECUTABLE_PATH}")
