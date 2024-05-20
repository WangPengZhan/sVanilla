# 项目使用
function(deployqt arg_target)
    find_program(qt_deployqt_executable NAMES windeployqt macdeployqt linuxdeployqt HINTS "${qt_bin_directory}")

    if (NOT EXISTS "${qt_deployqt_executable}")
        message("can not find the deployqt tools")
        return()
    endif ()

    # 获取组件参数列表
    # ${ARGN}是一个内置变量，表示传递给函数或宏的所有未命名参数
    set(qt_components ${ARGN})

    if (WIN32)
        add_custom_command(TARGET ${arg_target}
            POST_BUILD
            COMMAND
            "${qt_deployqt_executable}"
            --libdir "$<TARGET_FILE_DIR:${arg_target}>"
            --plugindir "$<TARGET_FILE_DIR:${arg_target}>"
            --no-translations
            --no-compiler-runtime
            --no-system-d3d-compiler
            --no-opengl-sw
            $<$<CONFIG:Debug>:--pdb>
            "$<TARGET_FILE:${arg_target}>"
        )

        # 根据find_package后面的component拷贝需要的动态库(防止遗漏)
        foreach (component IN LISTS qt_components)
            get_qt_library_target(${component} qt_component_target)
            add_custom_command(TARGET ${arg_target}
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_FILE:${qt_component_target}>"
                "$<TARGET_FILE_DIR:${arg_target}>"
            )
        endforeach ()
    elseif (APPLE)
        get_target_property(mac_app_dir ${arg_target} BINARY_DIR)
        set(mac_app_path "${mac_app_dir}/${arg_target}.app")
        add_custom_command(TARGET ${arg_target}
            POST_BUILD
            COMMAND
            "${qt_deployqt_executable}"
            "${mac_app_path}"
            -always-overwrite
        )
        find_program(CMAKE_INSTALL_NAME_TOOL NAMES install_name_tool)

        set(FREETYPE_PATH "${mac_app_dir}/${arg_target}.app/Contents/Frameworks/libfreetype.6.dylib")

        add_custom_command(TARGET ${arg_target}
            POST_BUILD
            COMMAND
            [ -f "${FREETYPE_PATH}" ]
            &&
            ${CMAKE_INSTALL_NAME_TOOL} -change
            @loader_path/../../../../opt/libpng/lib/libpng16.16.dylib
            @loader_path/libpng16.16.dylib
            ${FREETYPE_PATH}
            ||
            echo "can not find ${FREETYPE_PATH}"
        )
    elseif (UNIX)
        add_custom_command(TARGET ${arg_target}
            POST_BUILD
            COMMAND
            "${qt_deployqt_executable}"
            "$<TARGET_FILE:${arg_target}>"
            -always-overwrite
            -unsupported-allow-new-glibc
        )
    endif ()
endfunction()