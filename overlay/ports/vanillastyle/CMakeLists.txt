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
set(LIBRARY_NAME VanillaStyle)

add_library(${LIBRARY_NAME} SHARED)
target_sources(
    ${LIBRARY_NAME}
    PRIVATE
    ${SRC}
    ${RESOURCES}
    ${HEADER}
)

target_link_libraries(${LIBRARY_NAME}
    PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Svg
    PRIVATE
    nlohmann_json::nlohmann_json
)

target_include_directories(
    ${LIBRARY_NAME}
    PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${INCLUDE_DIR}>
)

set_target_properties(
    ${LIBRARY_NAME}
    PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    OUTPUT_NAME ${LIBRARY_NAME}
    PROJECT_LABEL ${LIBRARY_NAME}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    IMPORTED_CONFIGURATIONS "RELEASE;DEBUG"
    DEBUG_POSTFIX _debug
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)

install(TARGETS ${LIBRARY_NAME}
    EXPORT ${LIBRARY_NAME}Targets
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
    ${CMAKE_BINARY_DIR}/${LIBRARY_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# export cmake target files
export(EXPORT ${LIBRARY_NAME}Targets
    FILE ${CMAKE_BINARY_DIR}/${LIBRARY_NAME}Targets.cmake
)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/${LIBRARY_NAME}Config.cmake.in
    ${CMAKE_BINARY_DIR}/${LIBRARY_NAME}Config.cmake
    INSTALL_DESTINATION lib/cmake/${LIBRARY_NAME}
)

install(EXPORT ${LIBRARY_NAME}Targets
    FILE ${LIBRARY_NAME}Targets.cmake
    NAMESPACE ${LIBRARY_NAME}::
    DESTINATION lib/cmake/${LIBRARY_NAME}
)

# Install version and config files
install(FILES
    ${CMAKE_BINARY_DIR}/${LIBRARY_NAME}Config.cmake
    ${CMAKE_BINARY_DIR}/${LIBRARY_NAME}ConfigVersion.cmake
    DESTINATION lib/cmake/${LIBRARY_NAME}
)
