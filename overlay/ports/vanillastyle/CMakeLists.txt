find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Svg)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Svg)
find_package(nlohmann_json CONFIG REQUIRED)

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
set(LIBRAY_NAME VanillaStyle)

add_library(${LIBRAY_NAME} SHARED)
target_sources(
    ${LIBRAY_NAME}
    PRIVATE
    ${SRC}
    ${RESOURCES}
    ${HEADER}
)

target_link_libraries(${LIBRAY_NAME}
    PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Svg
    PRIVATE
    nlohmann_json::nlohmann_json
)

target_include_directories(
    ${LIBRAY_NAME}
    PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${INCLUDE_DIR}>
)

set_target_properties(
    ${LIBRAY_NAME}
    PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    OUTPUT_NAME ${LIBRAY_NAME}
    PROJECT_LABEL ${LIBRAY_NAME}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    DEBUG_POSTFIX _debug
)

install(TARGETS ${LIBRAY_NAME}
    EXPORT ${LIBRAY_NAME}Targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# install headers
install(DIRECTORY include/ DESTINATION include)

include(CMakePackageConfigHelpers)

# add version file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${LIBRAY_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# export cmake target files
export(EXPORT ${LIBRAY_NAME}Targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/${LIBRAY_NAME}Targets.cmake
)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/${LIBRAY_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${LIBRAY_NAME}Config.cmake
    INSTALL_DESTINATION lib/cmake/${LIBRAY_NAME}
)

install(EXPORT ${LIBRAY_NAME}Targets
    FILE ${LIBRAY_NAME}Targets.cmake
    NAMESPACE ${LIBRAY_NAME}::
    DESTINATION lib/cmake/${LIBRAY_NAME}
)

# Install version and config files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${LIBRAY_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${LIBRAY_NAME}ConfigVersion.cmake
    DESTINATION lib/cmake/${LIBRAY_NAME}
)
