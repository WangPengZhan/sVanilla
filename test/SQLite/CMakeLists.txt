set(SQLite_test SQLite_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)
set(SOURCES ${TEST_SOURCES})

find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

set(SQLite_Test_Lib SQLite_Test_Lib)
file(GLOB_RECURSE LIB_SOURCES LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/Sqlite/*.h ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/Sqlite/*.cpp)
add_library(${SQLite_Test_Lib} STATIC ${LIB_SOURCES})


add_executable(${SQLite_test} ${SOURCES})
add_test(NAME ${SQLite_test} COMMAND ${SQLite_test})
add_compile_definitions(TEST_PROJ_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}")
add_dependencies(${SQLite_test} GTest::gtest_main ${SQLite_Test_Lib})
target_link_libraries(${SQLite_test} PRIVATE GTest::gtest_main ${SQLite_Test_Lib})

target_link_libraries(${SQLite_Test_Lib}
    PRIVATE
        spdlog::spdlog
        unofficial::sqlite3::sqlite3
)

target_link_libraries(${SQLite_test}
    PRIVATE
        spdlog::spdlog
        unofficial::sqlite3::sqlite3
)
