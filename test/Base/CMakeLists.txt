set(TEST_NAME Base_test)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS *.h *.cpp)

add_executable(${TEST_NAME} ${TEST_SOURCES})
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

# Base 是纯头文件模块（没有库目标），测试只需要把 sVanilla/src 加进 include 路径。
target_include_directories(${TEST_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/sVanilla/src
)

target_link_libraries(${TEST_NAME}
    PRIVATE
        GTest::gtest_main
)
