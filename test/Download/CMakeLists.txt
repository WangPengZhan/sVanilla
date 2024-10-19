set(TESTED_LIB Download)
set(TEST_NAME ${TESTED_LIB}_test)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)
set(SOURCES ${TEST_SOURCES})

add_executable(${TEST_NAME} ${SOURCES})
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

add_compile_definitions(TEST_PROJ_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}")

include_directories(
    ${CMAKE_SOURCE_DIR}/sVanilla/src
)

set(QT_COMPONENTS
    Core
    Gui
    Widgets
    Network
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_COMPONENTS} LinguistTools)
find_package(spdlog REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(CURL REQUIRED)


foreach (COMPONENTS ${QT_COMPONENTS})
    set(QT_LIBS ${QT_LIBS} Qt${QT_VERSION_MAJOR}::${COMPONENTS})
endforeach ()
target_link_libraries(${TEST_NAME}
    PRIVATE
        GTest::gtest_main
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        CURL::libcurl
        ${QT_LIBS}
)
