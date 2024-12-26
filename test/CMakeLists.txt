
enable_testing()
include(GoogleTest)

add_compile_definitions(TEST_PROJ_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}")
find_package(GTest CONFIG REQUIRED)

option(SQLite_test "ENABLE SQLite test" ON)
if(SQLite_test)
    add_subdirectory(SQLite)
endif()

option(ThreadPool_test "ENABLE TheadPool_test test" ON)
if(ThreadPool_test)
    add_subdirectory(ThreadPool)
endif()

option(Zipper_test "ENABLE Zipper_test test" ON)
if(Zipper_test)
    add_subdirectory(Zipper)
endif()

