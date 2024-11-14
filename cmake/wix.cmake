function(generator_wix)
    if (WIN32)
        set(ICON_RESOURCE ${CMAKE_SOURCE_DIR}/sVanilla/resource/appIcon/${TARGET_NAME}.ico)
        math(EXPR PROJECT_VERSION_NUMBER "${PROJECT_VERSION_MAJOR} * 10000 + ${PROJECT_VERSION_MINOR} * 100 + ${PROJECT_VERSION_PATCH}")
        message(STATUS "Computed version number: ${PROJECT_VERSION_NUMBER}")
        configure_file(
            "${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}Info.wsi.in"
            "${CMAKE_BINARY_DIR}/${PROJECT_NAME}Info.wsi"
            @ONLY
        )
    endif()
endfunction()
