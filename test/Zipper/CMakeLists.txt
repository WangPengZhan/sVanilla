set(TESTED_LIB Zipper)
set(TEST_NAME ${TESTED_LIB}_test)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)
set(SOURCES ${TEST_SOURCES})

add_executable(${TEST_NAME} ${SOURCES})
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

add_compile_definitions(TEST_PROJ_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}")

include_directories(
    ${ZLIB_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/sVanilla/src
)

find_package(spdlog REQUIRED)
find_package(unofficial-minizip CONFIG REQUIRED)
find_package(ZLIB)

link_directories(${ZLIB_LIB})

target_link_libraries(${TEST_NAME}
    PRIVATE
        ${TESTED_LIB}
        GTest::gtest_main
        spdlog::spdlog
        unofficial::minizip::minizip
        ZLIB::ZLIB
)
