function(apptranslate QT_COMPMENTS)
    set(QT_TRANSLATES_DIR ${Qt${QT_VERSION_MAJOR}_DIR}/../../../translations)
    message("QT_TRANSLATE ${QT_TRANSLATE}")
    set(LANGUAGES
        zh_CN
        en
        zh_TW
    )

    set(QT_LOWERCASE_COMPONENTS)
    foreach(COMPONENT ${QT_COMPONENTS})
        string(TOLOWER "${COMPONENT}" LOWERCASE_COMPONENT)
        list(APPEND QT_LOWERCASE_COMPONENTS ${LOWERCASE_COMPONENT})
    endforeach()
    
    set(QT_TRANSLATE_COMPONETS)
    foreach(COMPONENT ${QT_LOWERCASE_COMPONENTS})
        if (COMPONENT STREQUAL core OR COMPONENT STREQUAL gui OR COMPONENT STREQUAL widgets)
            list(APPEND QT_TRANSLATE_COMPONETS qtbase)
        else()
            list(APPEND QT_TRANSLATE_COMPONETS qt${COMPONENT})
        endif()
    endforeach()

    list(REMOVE_DUPLICATES QT_TRANSLATE_COMPONETS)

    foreach(LANGUAGE ${LANGUAGES})
        foreach(COMPONENT ${QT_TRANSLATE_COMPONETS})
            if (EXISTS ${QT_TRANSLATES_DIR}/${COMPONENT}_${LANGUAGE}.qm)
                add_custom_command(TARGET ${PROJECT_NAME} 
                    POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E make_directory 
                            $<TARGET_FILE_DIR:${PROJECT_NAME}>/translations/${LANGUAGE}/
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                            ${QT_TRANSLATES_DIR}/${COMPONENT}_${LANGUAGE}.qm
                            $<TARGET_FILE_DIR:${PROJECT_NAME}>/translations/${LANGUAGE}/
                )
            endif()
        endforeach()
    endforeach()
endfunction()
