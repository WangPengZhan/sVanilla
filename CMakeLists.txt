cmake_minimum_required(VERSION 3.15)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/vcpkg/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
set(VCPKG_APPLOCAL_DEPS ON)


project(sVanilla VERSION 1.0.0 LANGUAGES CXX)

# if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
# endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)

cmake_policy(SET CMP0087 NEW)

# qt
set(QT_COMPONENTS
    Core
    Gui
    Widgets
    Network
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_COMPONENTS} LinguistTools)
find_package(ZLIB)
find_package(unofficial-breakpad CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
set(nlohmann-json_IMPLICIT_CONVERSIONS OFF)
find_package(spdlog CONFIG REQUIRED)
find_package(CURL REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

add_subdirectory(ThirdParty/FramelessHelper)
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/sVanilla/src
)

file(GLOB_RECURSE QTS "sVanilla/translate/*.ts")
file(GLOB_RECURSE QRC "sVanilla/resource/*.qrc")
file(GLOB_RECURSE SOURCES "sVanilla/src/*.cpp" "sVanilla/src/*.h" "sVanilla/src/*.ui")

message(STATUS "${SOURCES}")
set(ui ${CMAKE_SOURCE_DIR}/ui)
set(CMAKE_AUTOUIC_SEARCH_PATHS "ui")


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE release CACHE STRING "Build Type" FORCE)
endif()

# add_subdirectory(ThirdParty/ads EXCLUDE_FROM_ALL)
# add_subdirectory(ThirdParty/SARibbon)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${QT_MOC}
    ${UI_WRAP}
    ${UIS}
    ${QRC}
)


if(${QT_VERSION_MAJOR} EQUAL 5)
    qt5_add_translation(${PROJECT_NAME} TS_FILES ${QTS})
else()
    qt_add_translations(${PROJECT_NAME} TS_FILES ${QTS})
endif()

foreach (COMPONENTS ${QT_COMPONENTS})
    set(QT_LIBS ${QT_LIBS} Qt${QT_VERSION_MAJOR}::${COMPONENTS})
endforeach ()


target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${QT_LIBS}
        nlohmann_json::nlohmann_json
        CURL::libcurl
        spdlog::spdlog
        unofficial::breakpad::libbreakpad
        unofficial::breakpad::libbreakpad_client
        unofficial::sqlite3::sqlite3
        FramelessHelper::Core
        FramelessHelper::Widgets
)


set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# add_dependencies(${PROJECT_NAME} ads::qt6advanceddocking)
# add_dependencies(${PROJECT_NAME} SARibbonBar)

# add_custom_command(TARGET ${PROJECT_NAME}
#     POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:ads::qt6advanceddocking> $<TARGET_FILE_DIR:${PROJECT_NAME}>
#         COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/public $<TARGET_FILE_DIR:${PROJECT_NAME}>/public
# )

include(cmake/qtdeploy.cmake)
deployqt(${PROJECT_NAME})

# install
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}> DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})

# install(FILES "$<TARGET_FILE:ads::qt6advanceddocking>" DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})

