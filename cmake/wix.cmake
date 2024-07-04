function(generator_wix)
    if (WIN32)
        configure_file(
            "${CMAKE_SOURCE_DIR}/cmake/sVanillaInfo.wsi.in"
            "${CMAKE_BINARY_DIR}/sVanillaInfo.wsi"
            @ONLY
        )
    endif()
endfunction()
