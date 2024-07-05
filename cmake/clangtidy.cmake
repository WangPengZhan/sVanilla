function(clangtidy)
    find_program(CLANG_TIDY_EXE clang-tidy DOC "Path to clang-tidy executable" )
    if (NOT CLANG_TIDY_EXE)
        set(CLANG_TIDY_EXE "D:/Qt/Tools/QtCreator/bin/clang/bin/clang-tidy.exe")
    endif()
    message("CLANG_TIDY_EXE: ${CLANG_TIDY_EXE}")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    message("CMAKE_EXPORT_COMPILE_COMMANDS: ${CMAKE_EXPORT_COMPILE_COMMANDS}")
    set_target_properties(${PROJECT_NAME}
        PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXE};--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy;"
    )
endfunction(clangtidy)
