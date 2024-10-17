set(Zipper_test Zipper_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)
set(SOURCES ${TEST_SOURCES})

find_package(spdlog REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(CURL REQUIRED)
find_package(unofficial-minizip CONFIG REQUIRED)
find_package(ZLIB)

add_executable(${Zipper_test} ${SOURCES})
add_test(NAME ${Zipper_test} COMMAND ${Zipper_test})

add_compile_definitions(TEST_PROJ_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}")

include_directories(
    ${ZLIB_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/sVanilla/src
)
link_directories(${ZLIB_LIB})
message(STATUS "zlib include dir: ${ZLIB_LIB}")
target_link_libraries(${Zipper_test}
    PRIVATE
        ZLIB::ZLIB
)

target_link_libraries(${Zipper_test}
    PRIVATE
        GTest::gtest_main
        spdlog::spdlog
        unofficial::minizip::minizip
        Zipper
)

# add_custom_command(TARGET ${Zipper_test}
#     POST_BUILD
#         # COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:minizip> $<TARGET_FILE_DIR:${Zipper_test}>
# )
