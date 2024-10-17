set(SQLite_test SQLite_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)
file(GLOB_RECURSE TESTED_SOURCES LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/Sqlite/*.h ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/Sqlite/*.cpp)
set(SOURCES ${TEST_SOURCES} ${TESTED_SOURCES})

find_package(unofficial-sqlite3 CONFIG REQUIRED)

add_executable(${SQLite_test} ${SOURCES})
add_test(NAME ${SQLite_test} COMMAND ${SQLite_test})
add_compile_definitions(TEST_PROJ_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(${SQLite_test}
    PRIVATE
        GTest::gtest_main
        spdlog
        unofficial::sqlite3::sqlite3
)
