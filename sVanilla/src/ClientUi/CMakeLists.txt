option(ENABLE_DEPLOY "Enable deploy" ON)

# qt
set(QT_COMPONENTS
    Core
    Gui
    Widgets
    Network
    Svg
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_COMPONENTS} LinguistTools)
find_package(ZLIB)
find_package(unofficial-breakpad CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
set(nlohmann-json_IMPLICIT_CONVERSIONS OFF)
find_package(CURL REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(spdlog REQUIRED)
find_package(unofficial-minizip CONFIG REQUIRED)
find_package(QWindowKit REQUIRED)
find_package(VanillaStyle REQUIRED)
find_package(OpenSSL REQUIRED)


file(GLOB_RECURSE QTS "${CMAKE_CURRENT_SOURCE_DIR}/../../translate/*.ts")
file(GLOB_RECURSE Q_EN_TS "${CMAKE_CURRENT_SOURCE_DIR}/../../translate/*_en.ts")
file(GLOB_RECURSE Q_ZH_CN_TS "${CMAKE_CURRENT_SOURCE_DIR}/../../translate/*_zh_CN.ts")
file(GLOB_RECURSE Q_ZH_TW_TS "${CMAKE_CURRENT_SOURCE_DIR}/../../translate/*_zh_TW.ts")
file(GLOB_RECURSE QRC "${CMAKE_CURRENT_SOURCE_DIR}/../../resource/*.qrc")
file(GLOB_RECURSE SOURCES "*.cpp" "*.h")
file(GLOB_RECURSE UI "*.ui")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${QT_MOC}
    ${UI}
    ${UIS}
    ${QRC}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

include(${CMAKE_SOURCE_DIR}/cmake/appicon.cmake)
appicon(${PROJECT_NAME} RESOURCE_FILES)

include(${CMAKE_SOURCE_DIR}/cmake/appversion.cmake)
appversion()

target_sources(${PROJECT_NAME} 
    PRIVATE  
        ${RESOURCE_FILES}
)

if (WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

if (${QT_VERSION_MAJOR} EQUAL 5)
    qt5_add_translation(${PROJECT_NAME} TS_FILES ${QTS})
else ()
    qt_add_translations(${PROJECT_NAME} TS_FILES ${QTS})
endif ()

foreach (COMPONENTS ${QT_COMPONENTS})
    set(QT_LIBS ${QT_LIBS} Qt${QT_VERSION_MAJOR}::${COMPONENTS})
endforeach ()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${QT_LIBS}
        nlohmann_json::nlohmann_json
        CURL::libcurl
        unofficial::breakpad::libbreakpad
        unofficial::breakpad::libbreakpad_client
        unofficial::sqlite3::sqlite3
        QWindowKit::Widgets
        unofficial::minizip::minizip
        spdlog::spdlog
        OpenSSL::SSL
        OpenSSL::Crypto
        VanillaStyle::VanillaStyle
        Aria2Net
        BiliApi
        Download
        FFmpeg
        Logger
        Login
        NetWork
        Plugin
        Sqlite
        SUI
        ThreadPool
        Util
        Zipper
)

include(${CMAKE_SOURCE_DIR}/cmake/wix.cmake)
generator_wix()

if (WIN32)
    set(TOOLS_PATH ${CMAKE_SOURCE_DIR}/tools/win)
elseif (APPLE)
    set(TOOLS_PATH ${CMAKE_SOURCE_DIR}/tools/mac)
elseif (UNIX AND NOT APPLE)
    set(TOOLS_PATH ${CMAKE_SOURCE_DIR}/tools/linux)
endif ()

add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${TOOLS_PATH} $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/sVanilla/resource/style $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=${CMAKE_SOURCE_DIR}
        ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/get_authors.py "$<TARGET_FILE_DIR:${PROJECT_NAME}>/authors.csv"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=${CMAKE_SOURCE_DIR}
        ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/get_licenses.py "$<TARGET_FILE_DIR:${PROJECT_NAME}>/licenses.json"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

if (ENABLE_DEPLOY)
    include(${CMAKE_SOURCE_DIR}/cmake/qtdeploy.cmake)
    deployqt(${PROJECT_NAME})
endif ()

include(InstallRequiredSystemLibraries)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY ${TOOLS_PATH}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/sVanilla/resource/style/ DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
install(FILES "$<TARGET_FILE_DIR:${PROJECT_NAME}>/licenses.json" DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
install(FILES "$<TARGET_FILE_DIR:${PROJECT_NAME}>/authors.csv" DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
if (APPLE)
    install(FILES "$<TARGET_FILE:${PROJECT_NAME}>" DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
endif()

file(GLOB VCPAKG_DEP_DIRS LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/ThirdParty/vcpkg/packages/*")
if (VCPAKG_DEP_DIRS)
    foreach (VCPAKG_DEP_DIR ${VCPAKG_DEP_DIRS})
        if (WIN32)
            set(LIB_PRX_PATH bin)
        elseif (APPLE)
            set(LIB_PRX_PATH lib)
        elseif (UNIX AND NOT APPLE)
            set(LIB_PRX_PATH lib)
        endif ()

        if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
            set(VCPKG_LIB ${VCPAKG_DEP_DIR}/debug/${LIB_PRX_PATH}/)
        else ()
            set(VCPKG_LIB ${VCPAKG_DEP_DIR}/${LIB_PRX_PATH}/)
        endif ()

        string(FIND "${VCPAKG_DEP_DIR}" "gtest" POS)
        if (EXISTS "${VCPKG_LIB}" AND "${POS}" EQUAL -1)
            add_custom_command(TARGET ${PROJECT_NAME}
                POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory ${VCPKG_LIB} $<TARGET_FILE_DIR:${PROJECT_NAME}>
            )
            install(DIRECTORY ${VCPKG_LIB} DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
        endif ()
    endforeach ()
endif ()

if (ENABLE_BUILD_PLUGINS)
    add_dependencies(${PROJECT_NAME} sVanillaPlugins)
    get_target_property(PLUGINS sVanillaPlugins _PLUGINS_)
endif()

if (PLUGINS)
    foreach (PLUGIN ${PLUGINS})
        add_custom_command(TARGET ${PROJECT_NAME}
            POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/plugin
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${PLUGIN}> $<TARGET_FILE_DIR:${PROJECT_NAME}>/plugin
        )
        install(FILES "$<TARGET_FILE:${PLUGIN}>" DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/plugin)
    endforeach ()
endif ()

include(${CMAKE_SOURCE_DIR}/cmake/apptranslate.cmake)
apptranslate(${QT_COMPONENTS})
if (Q_EN_TS)
    apptranslate_copy(${Q_EN_TS} "en")
endif()
if (Q_ZH_CN_TS)
    apptranslate_copy(${Q_ZH_CN_TS} "zh_CN")
endif()
if (Q_ZH_TW_TS)
    apptranslate_copy(${Q_ZH_TW_TS} "zh_TW")
endif()
