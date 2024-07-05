cmake_minimum_required(VERSION 3.15)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "")
set(VCPKG_APPLOCAL_DEPS ON)

project(sVanilla 
    VERSION 1.0.0 
    DESCRIPTION "A downloader of stream that may be video, audio or music."
    HOMEPAGE_URL "https://svanilla.app/"
    LANGUAGES CXX)

set(PROJECT_COMPANY_NAME "sVanilla Studio")
set(PROJECT_COPYRIGHT "Copyright © iveswang and other contributors")

# if(CMAKE_VERSION VERSION_LESS "3.7.0")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# endif()

# set(CMAKE_CXX_FLAGS "-U__FILE__ -D__FILE__='\"$(subst $(dir $<),,$<)\"'")
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Wno-builtin-macro-redefined SUPPORT_CXX_WNO_BUILTIN_MACRO_REDEFINED)
if (SUPPORT_CXX_WNO_BUILTIN_MACRO_REDEFINED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-builtin-macro-redefined")
else ()
    message("CXXCompiler can't support Wno-builtin-macro-redefined")
endif (SUPPORT_CXX_WNO_BUILTIN_MACRO_REDEFINED)

# MSVC并行编译 cmake -j xxx 对于MSVC没有作用
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/Gm->) #高版本已被废弃，但是低版本的Gm会影响并行
cmake_host_system_information(RESULT CPU_NUMBER_OF_LOGICAL_CORES QUERY NUMBER_OF_LOGICAL_CORES)
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP${CPU_NUMBER_OF_LOGICAL_CORES}>)

# MSVC release generate pdb
if (MSVC)
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
    SET(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
endif()

add_compile_definitions(UNICODE)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)

cmake_policy(SET CMP0087 NEW)
cmake_policy(SET CMP0079 NEW)

option(ENABLE_DEPLOY "Enable deploy" ON)
option(ENABLE_TEST "Enable Test" OFF)
option(ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)

# qt
set(QT_COMPONENTS
    Core
    Gui
    Widgets
    Network
    Svg
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Svg)
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
find_package(rapidfuzz REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/sVanilla/src
)

file(GLOB_RECURSE QTS "sVanilla/translate/*.ts")
file(GLOB_RECURSE Q_EN_TS "sVanilla/translate/*_en.ts")
file(GLOB_RECURSE Q_ZH_CN_TS "sVanilla/translate/*_zh_CN.ts")
file(GLOB_RECURSE Q_ZH_TW_TS "sVanilla/translate/*_zh_TW.ts")
file(GLOB_RECURSE QRC "sVanilla/resource/*.qrc")
file(GLOB_RECURSE SOURCES "sVanilla/src/*.cpp" "sVanilla/src/*.h")
file(GLOB_RECURSE UI "sVanilla/src/*.ui")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE release CACHE STRING "Build Type" FORCE)
endif ()

add_subdirectory(ThirdParty/sVanillaPlugins EXCLUDE_FROM_ALL)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if (ENABLE_TEST)
    enable_testing()
    add_subdirectory(test)
endif ()


add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${QT_MOC}
    ${UI}
    ${UIS}
    ${QRC}
)

include(cmake/appicon.cmake)
appicon(${PROJECT_NAME} RESOURCE_FILES)

include(cmake/appversion.cmake)
appversion()

target_sources(${PROJECT_NAME} 
    PRIVATE  
        ${RESOURCE_FILES}
)

if (WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

if (ENABLE_CLANG_TIDY)
    include(cmake/clangtidy.cmake)
    clangtidy()
endif()

# include(cmake/filemarco.cmake)
# redefine_file_macro(${PROJECT_NAME})

if (${QT_VERSION_MAJOR} EQUAL 5)
    qt5_add_translation(${PROJECT_NAME} TS_FILES ${QTS})
else ()
    qt_add_translations(${PROJECT_NAME} TS_FILES ${QTS})
endif ()

foreach (COMPONENTS ${QT_COMPONENTS})
    set(QT_LIBS ${QT_LIBS} Qt${QT_VERSION_MAJOR}::${COMPONENTS})
endforeach ()

add_dependencies(${PROJECT_NAME} sVanillaPlugins)

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
    rapidfuzz::rapidfuzz
)

# add_dependencies(${PROJECT_NAME} ads::qt6advanceddocking)
# add_dependencies(${PROJECT_NAME} SARibbonBar)

find_program(PYTHON_EXECUTABLE python3)
if (NOT PYTHON_EXECUTABLE)
    find_program(PYTHON_EXECUTABLE python)
endif ()
if (PYTHON_EXECUTABLE)
    message(STATUS "PYTHON_EXECUTABLE: ${PYTHON_EXECUTABLE}")
    set(INSTALL_DEPENDENCIES_COMMAND "${PYTHON_EXECUTABLE}" -m pip install -r ${CMAKE_CURRENT_SOURCE_DIR}/scripts/requirements.txt)
    execute_process(COMMAND ${INSTALL_DEPENDENCIES_COMMAND}
        RESULT_VARIABLE INSTALL_RESULT
        OUTPUT_VARIABLE INSTALL_OUTPUT
        ERROR_VARIABLE INSTALL_ERROR)
    if (NOT INSTALL_RESULT EQUAL 0)
        message(FATAL_ERROR "INSTALL_ERROR: ${INSTALL_ERROR}")
    endif ()
else ()
    message(FATAL_ERROR "can't find python")
endif ()

include(cmake/wix.cmake)
generator_wix()

if (WIN32)
    set(TOOLS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/tools/win)
elseif (APPLE)
    set(TOOLS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/tools/mac)
elseif (UNIX AND NOT APPLE)
    set(TOOLS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/tools/linux)
endif ()

add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${TOOLS_PATH} $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/sVanilla/resource/style $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}
        ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/get_authors.py "$<TARGET_FILE_DIR:${PROJECT_NAME}>/authors.csv"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}
        ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/get_licenses.py "$<TARGET_FILE_DIR:${PROJECT_NAME}>/licenses.json"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

if (ENABLE_DEPLOY)
    include(cmake/qtdeploy.cmake)
    deployqt(${PROJECT_NAME})
endif ()

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY ${TOOLS_PATH}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sVanilla/resource/style/ DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
install(DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>/ DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
install(FILES "$<TARGET_FILE_DIR:${PROJECT_NAME}>/licenses.json" DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
install(FILES "$<TARGET_FILE_DIR:${PROJECT_NAME}>/authors.csv" DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})

file(GLOB VCPAKG_DEP_DIRS LIST_DIRECTORIES true "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/vcpkg/packages/*")
if (VCPAKG_DEP_DIRS)
    foreach (VCPAKG_DEP_DIR ${VCPAKG_DEP_DIRS})
        if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
            set(VCPKG_LIB ${VCPAKG_DEP_DIR}/debug/bin/)
        else ()
            set(VCPKG_LIB ${VCPAKG_DEP_DIR}/bin/)
        endif ()

        string(FIND "${VCPAKG_DEP_DIR}" "gtest" POS)
        if (EXISTS ${VCPKG_LIB} AND ${POS} EQUAL 0)
            add_custom_command(TARGET ${PROJECT_NAME}
                POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory ${VCPKG_LIB} $<TARGET_FILE_DIR:${PROJECT_NAME}>
            )
            install(DIRECTORY ${VCPKG_LIB} DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
        endif ()
    endforeach ()
endif ()

get_target_property(PLUGINS sVanillaPlugins _PLUGINS_)
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

# include(InstallRequiredSystemLibraries)

include(cmake/apptranslate.cmake)
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
