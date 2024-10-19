set(TESTED_LIB SQLite)
set(TEST_NAME ${TESTED_LIB}_test)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)
set(SOURCES ${TEST_SOURCES})

find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_executable(${TEST_NAME} ${SOURCES})
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
add_compile_definitions(TEST_PROJ_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(${TEST_NAME}
    PRIVATE
        GTest::gtest_main
        spdlog::spdlog
        unofficial::sqlite3::sqlite3
)
