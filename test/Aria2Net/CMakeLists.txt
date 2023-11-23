set(Aria2Net_test Aria2Net_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)
file(GLOB_RECURSE TESTED_SOURCES LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/Aria2Net/*.h ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/Aria2Net/*.cpp)
set(SOURCES ${TEST_SOURCES} ${TESTED_SOURCES})

set(CMAKE_AUTOMOC ON)
# qt
set(QT_COMPONENTS
    Core
    Gui
    Widgets
    Network
)

find_package(spdlog CONFIG REQUIRED)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_COMPONENTS} LinguistTools)

add_executable(${Aria2Net_test} ${SOURCES})
add_test(NAME ${Aria2Net_test} COMMAND ${Aria2Net_test})
add_compile_definitions(TEST_PROJ_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}")
add_dependencies(${Aria2Net_test} GTest::gtest_main)

foreach (COMPONENTS ${QT_COMPONENTS})
    set(QT_LIBS ${QT_LIBS} Qt${QT_VERSION_MAJOR}::${COMPONENTS})
endforeach ()
target_link_libraries(${Aria2Net_test}
    PRIVATE
        GTest::gtest_main
        spdlog::spdlog
        ${QT_LIBS}
)
