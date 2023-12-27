set(Zipper_test Zipper_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)
file(GLOB_RECURSE TESTED_SOURCES LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/zipper/*.h ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/zipper/*.cpp)
# file(GLOB_RECURSE TESTED_DEPANDED_SOURCES LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/NetWork/*.h ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/NetWork/*.cpp)
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

add_executable(${Zipper_test} ${SOURCES})
add_test(NAME ${Zipper_test} COMMAND ${Zipper_test})
add_compile_definitions(TEST_PROJ_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}")
add_dependencies(${Zipper_test} GTest::gtest_main)

foreach (COMPONENTS ${QT_COMPONENTS})
    set(QT_LIBS ${QT_LIBS} Qt${QT_VERSION_MAJOR}::${COMPONENTS})
endforeach ()
target_link_libraries(${Zipper_test}
    PRIVATE
        GTest::gtest_main
        spdlog::spdlog
        minizip
)

add_custom_command(TARGET ${Zipper_test}
    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:minizip> $<TARGET_FILE_DIR:${Zipper_test}>
)
