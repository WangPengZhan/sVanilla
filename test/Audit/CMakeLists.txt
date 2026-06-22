set(TEST_NAME Audit_test)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)

add_executable(${TEST_NAME} ${TEST_SOURCES})
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

target_include_directories(${TEST_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/sVanilla/src
        ${CMAKE_SOURCE_DIR}/ThirdParty/sVanillaPluginCommon
        ${CMAKE_SOURCE_DIR}/ThirdParty/sVanillaPluginCommon/PluginCommon
)

target_link_libraries(${TEST_NAME}
    PRIVATE
        Download
        Plugin
        PluginCommon
        PluginDownload
        GTest::gtest_main
        spdlog::spdlog
)

add_custom_command(TARGET ${TEST_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:spdlog>
        $<TARGET_FILE_DIR:${TEST_NAME}>
)
