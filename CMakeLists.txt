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

set(CMAKE_INCLUDE_CURRENT_DIR ON)

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

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE release CACHE STRING "Build Type" FORCE)
endif ()

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

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core LinguistTools)

add_subdirectory(ThirdParty/sVanillaPlugins EXCLUDE_FROM_ALL)
add_subdirectory(sVanilla/src)

option(ENABLE_TEST "Enable Test" OFF)
option(ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)

if (ENABLE_TEST)
    enable_testing()
    add_subdirectory(test)
endif ()

if (ENABLE_CLANG_TIDY)
    include(${CMAKE_SOURCE_DIR}/cmake/clangtidy.cmake)
    clangtidy()
endif()
