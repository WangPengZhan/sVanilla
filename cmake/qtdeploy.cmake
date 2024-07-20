# 项目使用
function(deployqt arg_target)
    find_program(qt_deployqt_executable NAMES windeployqt macdeployqt linuxdeployqt HINTS "${qt_bin_directory}")

    if (WIN32 OR APPLE)
        if (NOT EXISTS "${qt_deployqt_executable}")
            message("can not find the deployqt tools")
            return()
        endif ()
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

        install(DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>/ DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
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
    elseif (UNIX)
        if (NOT EXISTS "${qt_deployqt_executable}")
            message("can not find the deployqt tools")
        else ()
            add_custom_command(TARGET ${arg_target}
                POST_BUILD
                    COMMAND
                        "${qt_deployqt_executable}"
                        "$<TARGET_FILE:${arg_target}>"
                        -always-overwrite
                        -unsupported-allow-new-glibc
            )
        endif ()

        install(DIRECTORY ${CMAKE_SOURCE_DIR}/tools/linux/ DESTINATION ${CMAKE_INSTALL_PREFIX}/${arg_target}/usr/bin)
        install(DIRECTORY ${CMAKE_SOURCE_DIR}/sVanilla/resource/style/ DESTINATION ${CMAKE_INSTALL_PREFIX}/${arg_target}/usr/bin)
        install(FILES "$<TARGET_FILE_DIR:${arg_target}>/licenses.json" DESTINATION ${CMAKE_INSTALL_PREFIX}/${arg_target}/usr/bin)
        install(FILES "$<TARGET_FILE_DIR:${arg_target}>/authors.csv" DESTINATION ${CMAKE_INSTALL_PREFIX}/${arg_target}/usr/bin)
    endif ()
endfunction()