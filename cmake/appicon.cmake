function(appicon TARGET_NAME RESOURCE_FILES)
    if(WIN32)
        set(ICON_RESOURCE sVanilla/resource/app.icns)
        set(RC_CONTENT "IDI_ICON1    ICON    DISCARDABLE     \"${ICON_RESOURCE}\"")
        set(RC_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.rc)
        file(WRITE ${RC_FILE} "${RC_CONTENT}")
        set(RESOURCE_FILES ${ICON_RESOURCE} ${TARGET_NAME}.rc PARENT_SCOPE)
    elseif(APPLE)
        set(ICON_FILE sVanilla/resource/app.ico)
        get_filename_component(ICON_FILE_NAME ${ICON_FILE} NAME)
        set(MACOSX_BUNDLE_ICON_FILE ${ICON_FILE_NAME})
        set_source_files_properties(${ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
        set(RESOURCE_FILES ${ICON_FILE} PARENT_SCOPE)
    elseif(UNIX AND NOT APPLE)
        set(ICON_ROOT $<TARGET_FILE_DIR:${TARGET_NAME}>/share)
        set(ICON_RESOURCE "${ICON_FILE}")
        set(DESKTOP_CONTENT "[Desktop Entry]
Version=1.5
Type=Application
Name=${TARGET_NAME}
Comment=A beautiful downloader
Exec=${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}
Icon=${TARGET_NAME}
MimeType=image/x-foo;
Categories=Utility;Network;Qt;")
        set(DESKTOP_FILE ${ICON_ROOT}/applications/${TARGET_NAME}.desktop)
        message("DESKTOP_FILE: ${DESKTOP_FILE}")
        file(GENERATE OUTPUT ${DESKTOP_FILE} CONTENT "${DESKTOP_CONTENT}")
        set(ICON_DIR "${ICON_ROOT}/icon/")
        add_custom_command(TARGET ${TARGET_NAME}
            POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory ${ICON_DIR}
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/sVanilla/resource/sVanilla.svg ${ICON_DIR}
        )
    endif()
endfunction()