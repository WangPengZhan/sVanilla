set(TARGET_EXECUTABLE_PATH "${EXECUTABLE_PATH}")

function(kill_process_by_path executable_path)
    get_filename_component(process_name "${executable_path}" NAME)

    if(WIN32)
        string(REPLACE "/" "\\\\" executable_path "${executable_path}.exe")

        execute_process(
            COMMAND powershell -Command "
                Get-Process -Name '${process_name}' -ErrorAction SilentlyContinue |
                Stop-Process -Force;
                Start-Sleep -Milliseconds 200
            "
            RESULT_VARIABLE result
            ERROR_QUIET
        )
    elseif(APPLE OR UNIX)
        execute_process(
            COMMAND bash -c "ps -eo pid,args | grep '${executable_path}' | grep -v grep | awk '{print \$1}' | xargs -r kill -9"
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
