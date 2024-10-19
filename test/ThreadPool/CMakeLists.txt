set(TESTED_LIB ThreadPool)
set(TEST_NAME ${TESTED_LIB}_test)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)
set(SOURCES ${TEST_SOURCES})

set(CMAKE_AUTOMOC ON)

# qt
set(QT_COMPONENTS
    Core
    Gui
    Widgets
    Network
)

include_directories(
    ${CMAKE_SOURCE_DIR}/sVanilla/src
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_COMPONENTS} LinguistTools)
find_package(spdlog REQUIRED)

add_executable(${TEST_NAME} ${SOURCES})
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
add_compile_definitions(TEST_PROJ_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}")

foreach (COMPONENTS ${QT_COMPONENTS})
    set(QT_LIBS ${QT_LIBS} Qt${QT_VERSION_MAJOR}::${COMPONENTS})
endforeach ()

target_link_libraries(${TEST_NAME}
    PRIVATE
        ${TESTED_LIB}
        GTest::gtest_main
        spdlog::spdlog
        ${QT_LIBS}
)
