function(generator_wix)
    if (WIN32)
        configure_file(
            "${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}Info.wsi.in"
            "${CMAKE_BINARY_DIR}/${PROJECT_NAME}Info.wsi"
            @ONLY
        )
    endif()
endfunction()
