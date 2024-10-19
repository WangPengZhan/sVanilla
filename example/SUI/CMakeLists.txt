cmake_minimum_required(VERSION 3.15)

set(SUI_EXAMPLE sui_example)

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


include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src
)

file(GLOB_RECURSE QTS "src/*.ts")
file(GLOB_RECURSE QRC "/resource/*.qrc")
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")
file(GLOB_RECURSE UI "src/*.ui")


if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE release CACHE STRING "Build Type" FORCE)
endif ()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_executable(${SUI_EXAMPLE}
    ${SOURCES}
    ${QT_MOC}
    ${UI}
    ${UIS}
    ${QRC}
)

if (WIN32)
    set_target_properties(${SUI_EXAMPLE} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

if (${QT_VERSION_MAJOR} EQUAL 5)
    qt5_add_translation(${SUI_EXAMPLE} TS_FILES ${QTS})
else ()
    qt_add_translations(${SUI_EXAMPLE} TS_FILES ${QTS})
endif ()

foreach (COMPONENTS ${QT_COMPONENTS})
    set(QT_LIBS ${QT_LIBS} Qt${QT_VERSION_MAJOR}::${COMPONENTS})
endforeach ()


target_link_libraries(${SUI_EXAMPLE}
    PRIVATE
        ${QT_LIBS}
        SUI
)

