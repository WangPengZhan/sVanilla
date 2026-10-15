set(TEST_NAME Plugin_test)
set(TESTED_LIB Plugin)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS *.h *.cpp)

add_executable(${TEST_NAME} ${TEST_SOURCES})
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

find_package(nlohmann_json CONFIG REQUIRED)

target_include_directories(${TEST_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/sVanilla/src
        ${CMAKE_SOURCE_DIR}/ThirdParty/sVanillaPluginCommon
        ${CMAKE_SOURCE_DIR}/ThirdParty/sVanillaPluginCommon/PluginCommon
        ${CMAKE_SOURCE_DIR}/ThirdParty/sVanillaPluginCommon/include
)

# 注意：用 GTest::gtest（不含 main），main() 由本测试提供，用于在启动时注册具名 logger。
target_link_libraries(${TEST_NAME}
    PRIVATE
        ${TESTED_LIB}
        PluginCommon
        PluginDownload
        GTest::gtest
        spdlog::spdlog
        nlohmann_json::nlohmann_json
)

add_custom_command(TARGET ${TEST_NAME}
    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:spdlog> $<TARGET_FILE_DIR:${TEST_NAME}>
)
