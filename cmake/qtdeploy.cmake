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
        endforeach()
    elseif(APPLE)
        add_custom_command(TARGET ${arg_target} 
            POST_BUILD 
                COMMAND
                    "${qt_deployqt_executable}"
                    "$<TARGET_FILE:${arg_target}>"
                    -always-overwrite
        )
    elseif(UNIX)
        add_custom_command(TARGET ${arg_target} 
            POST_BUILD 
                COMMAND
                    "${qt_deployqt_executable}"
                    "$<TARGET_FILE:${arg_target}>"
                    -always-overwrite
                    -unsupported-allow-new-glibc
        )
    endif()
    
    # 根据find_package后面的component拷贝需要的动态库(防止遗漏)
    foreach (component IN LISTS qt_components)
        get_qt_library_target(${component} qt_component_target)
        add_custom_command(TARGET ${arg_target} 
            POST_BUILD 
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "$<TARGET_FILE:${qt_component_target}>"
                    "$<TARGET_FILE_DIR:${arg_target}>"
        )
    endforeach()
endfunction()