function(appicon TARGET_NAME RESOURCE_FILES)
    if(WIN32)
        set(ICON_RESOURCE ${CMAKE_SOURCE_DIR}/sVanilla/resource/appIcon/${TARGET_NAME}.ico)
        set(RC_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.rc)
        configure_file(${CMAKE_SOURCE_DIR}/cmake/${TARGET_NAME}.rc.in ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.rc)
        set(RESOURCE_FILES ${ICON_RESOURCE} ${TARGET_NAME}.rc PARENT_SCOPE)
    elseif(APPLE)
        set(ICON_FILE ${CMAKE_SOURCE_DIR}/sVanilla/resource/appIcon/${TARGET_NAME}.icns)
        get_filename_component(ICON_FILE_NAME ${ICON_FILE} NAME)
        set_target_properties(${PROJECT_NAME} PROPERTIES
            MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
            MACOSX_BUNDLE_GUI_IDENTIFIER svanilla.app
            MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
            MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION}
            MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
            MACOSX_BUNDLE_ICON_FILE ${ICON_FILE_NAME}
            MACOSX_BUNDLE_COPYRIGHT ${PROJECT_COPYRIGHT}
            RESOURCE ${ICON_FILE}
            MACOSX_BUNDLE TRUE
        )
        set(RESOURCE_FILES ${ICON_FILE} PARENT_SCOPE)
    elseif(UNIX AND NOT APPLE)
        include(GNUInstallDirs)
        set(ICON_ROOT ${CMAKE_CURRENT_BINARY_DIR}/share)
        set(DESKTOP_FILE ${ICON_ROOT}/applications/${TARGET_NAME}.desktop)
        message("DESKTOP_FILE: ${DESKTOP_FILE}")
        configure_file(${CMAKE_SOURCE_DIR}/cmake/${TARGET_NAME}.desktop.in ${DESKTOP_FILE})
        file(GENERATE OUTPUT ${DESKTOP_FILE} CONTENT "${DESKTOP_CONTENT}")
        set(ICON_DIR "${ICON_ROOT}/icon/")
        add_custom_command(TARGET ${TARGET_NAME}
            POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory ${ICON_DIR}
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/sVanilla/resource/appIcon/${TARGET_NAME}.svg ${ICON_DIR}
        )
    endif()
endfunction()