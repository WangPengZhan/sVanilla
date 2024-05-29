function(appversion)
    find_program(GIT_EXECUTABLE git)
    if(NOT GIT_EXECUTABLE)
        message(FATAL_ERROR "Git was not found. Please install Git and ensure it is in your PATH.")
    endif()

    execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    string(TIMESTAMP BUILD_DATE "%Y-%m-%d")
    string(TIMESTAMP BUILD_TIME "%H:%M:%S")

    configure_file(${CMAKE_SOURCE_DIR}/cmake/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)
endfunction()
