set(FFmpeg_test FFmpeg_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)
file(GLOB_RECURSE TESTED_SOURCES LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/Aria2Net/*.h ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/Aria2Net/*.cpp)
file(GLOB_RECURSE TESTED_DEPANDED_SOURCES LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/NetWork/*.h ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/NetWork/*.cpp)
set(SOURCES ${TEST_SOURCES} ${TESTED_SOURCES} ${TESTED_DEPANDED_SOURCES})

set(CMAKE_AUTOMOC ON)
# qt
set(QT_COMPONENTS
    Core
    Gui
    Widgets
    Network
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_COMPONENTS} LinguistTools)

add_executable(${FFmpeg_test} ${SOURCES})
add_test(NAME ${FFmpeg_test} COMMAND ${FFmpeg_test})
add_compile_definitions(TEST_PROJ_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}")
add_dependencies(${FFmpeg_test} GTest::gtest_main)

foreach (COMPONENTS ${QT_COMPONENTS})
    set(QT_LIBS ${QT_LIBS} Qt${QT_VERSION_MAJOR}::${COMPONENTS})
endforeach ()
target_link_libraries(${FFmpeg_test}
    PRIVATE
        GTest::gtest_main
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        CURL::libcurl
        ${QT_LIBS}
)
