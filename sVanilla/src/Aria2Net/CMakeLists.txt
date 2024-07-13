cmake_minimum_required(VERSION 3.15)

project(Aria2Net 
    VERSION 1.0.0 
    DESCRIPTION "Aria2 cpp wrapper"
    LANGUAGES CXX)

set(QT_COMPONENTS
    Core
    Gui
    Widgets
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_COMPONENTS})
find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog REQUIRED)

set(nlohmann-json_IMPLICIT_CONVERSIONS OFF)

file(GLOB_RECURSE SOURCES "*.cpp" "*.h")

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

foreach (COMPONENTS ${QT_COMPONENTS})
    set(QT_LIBS ${QT_LIBS} Qt${QT_VERSION_MAJOR}::${COMPONENTS})
endforeach ()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${QT_LIBS}
        nlohmann_json::nlohmann_json
        Util
        spdlog::spdlog
)
