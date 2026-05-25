set(TARGET_EXECUTABLE_PATH "${EXECUTABLE_PATH}")

function(kill_process_by_path executable_path)
    if(NOT executable_path OR executable_path STREQUAL "")
        message(WARNING "Empty executable path provided, skipping process termination.")
        return()
    endif()

    get_filename_component(process_name "${executable_path}" NAME)

    if(WIN32)
        string(REPLACE "/" "\\" executable_path "${executable_path}.exe")

        execute_process(
            COMMAND powershell -Command "
                Get-Process -Name '${process_name}' -ErrorAction SilentlyContinue |
                Stop-Process -Force;
            "
            RESULT_VARIABLE result
            ERROR_QUIET
        )
    elseif(APPLE OR UNIX)
        if(NOT EXISTS "${executable_path}")
            message("Executable path does not exist: ${executable_path}, skipping process termination.")
            return()
        endif()

        execute_process(
            COMMAND bash -c "pkill -f '^${executable_path}( |$)' || true"
            RESULT_VARIABLE result
            ERROR_QUIET
        )
    endif()

    if(result EQUAL 0)
        message(STATUS "Successfully terminated process at path: ${executable_path}")
    else()
        message(WARNING "Failed to terminate process at path: ${executable_path} or process not found.")
    endif()
endfunction()

kill_process_by_path("${TARGET_EXECUTABLE_PATH}")
