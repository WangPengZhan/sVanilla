

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/googletest/googletest/include
)

add_compile_definitions(TEST_PROJ_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}")
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/googletest test_binary_dir)

option(SQLite_test "ENABLE SQLite test" ON)
if(SQLite_test)
    add_subdirectory(SQLite)
endif()

