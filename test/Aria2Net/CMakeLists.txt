set(TEST_NAME Aria2Net_test)
set(TESTED_LIB Aria2Net)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS *.h *.cpp)

set(QT_COMPONENTS
    Core
    Gui
    Widgets
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_COMPONENTS})
find_package(nlohmann_json CONFIG REQUIRED)

set(CMAKE_AUTOMOC ON)

add_executable(${TEST_NAME} ${TEST_SOURCES})
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

set_tests_properties(${TEST_NAME} PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

target_include_directories(${TEST_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/sVanilla/src
)

foreach (COMPONENTS ${QT_COMPONENTS})
    set(QT_LIBS ${QT_LIBS} Qt${QT_VERSION_MAJOR}::${COMPONENTS})
endforeach ()

# AriaServer 使用 BaseQt 的 TransceiverForQt，而 Aria2Net 自身没有链接 BaseQt，
# 因此这里必须补上，否则 TransceiverForQt 的符号无法解析。
target_link_libraries(${TEST_NAME}
    PRIVATE
        ${TESTED_LIB}
        BaseQt
        GTest::gtest
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        ${QT_LIBS}
)

add_custom_command(TARGET ${TEST_NAME}
    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:spdlog> $<TARGET_FILE_DIR:${TEST_NAME}>
)
