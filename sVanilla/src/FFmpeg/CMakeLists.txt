cmake_minimum_required(VERSION 3.15)

project(FFmpeg 
    VERSION 1.0.0 
    LANGUAGES CXX)

set(QT_COMPONENTS
    Core
    Gui
    Widgets
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_COMPONENTS})
find_package(spdlog REQUIRED)

file(GLOB_RECURSE SOURCES "*.cpp" "*.h")

add_library(${PROJECT_NAME} STATIC ${SOURCES})


foreach (COMPONENTS ${QT_COMPONENTS})
    set(QT_LIBS ${QT_LIBS} Qt${QT_VERSION_MAJOR}::${COMPONENTS})
endforeach ()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${QT_LIBS}
        spdlog::spdlog
)
