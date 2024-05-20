find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Svg)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Svg)

add_library(${PROJECT_NAME} SHARED)

include(GNUInstallDirs)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
#include(Package)
find_package(nlohmann_json CONFIG REQUIRED)
set(nlohmann-json_IMPLICIT_CONVERSIONS OFF)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    nlohmann_json::nlohmann_json
)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/res)

file(GLOB_RECURSE SRC
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/*.h"
)

file(GLOB_RECURSE HEADER
    "${INCLUDE_DIR}/*.h"
)
source_group(include FILES ${HEADER})

set(RESOURCES
    ${RESOURCE_DIR}/VanillaStyle.qrc
    ${RESOURCE_DIR}/VanillaStyleRobotoFont.qrc
    $<$<PLATFORM_ID:Windows>:${RESOURCE_DIR}/VanillaStyleFont.qrc>
)

target_sources(
    ${PROJECT_NAME}
    PRIVATE
    ${SRC}
    ${RESOURCES}
    ${HEADER}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Svg
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    OUTPUT_NAME ${PROJECT_NAME}
    PROJECT_LABEL ${PROJECT_NAME}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    IMPORTED_CONFIGURATIONS "RELEASE;DEBUG"
    DEBUG_POSTFIX _debug
    WINDOWS_EXPORT_ALL_SYMBOLS ON
    PUBLIC_HEADER "${HEADER}"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE VANILLA_LIBRARY)

include(Installing)