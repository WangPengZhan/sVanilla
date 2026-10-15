set(TEST_NAME SUI_test)
set(TESTED_LIB SUI)

set(QT_COMPONENTS
    Core
    Gui
    Widgets
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_COMPONENTS})

set(CMAKE_AUTOMOC ON)

# 只把被验证的 SUI 源文件编进测试。
# 整个 SUI 静态库会带出 Qt WebEngineCore / WebEngineWidgets 依赖，对纯控件测试是不必要的负担，
# 而 CMake 对静态库的 PRIVATE 依赖仍会作为链接需求传递给消费方。
set(SUI_SOURCES
    ${CMAKE_SOURCE_DIR}/sVanilla/src/SUI/DownloadCount.cpp
    ${CMAKE_SOURCE_DIR}/sVanilla/src/SUI/SlidingWaitBar.cpp
)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS *.h *.cpp)

add_executable(${TEST_NAME} ${TEST_SOURCES} ${SUI_SOURCES})
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

set_tests_properties(${TEST_NAME} PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

target_include_directories(${TEST_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/sVanilla/src
)

foreach (COMPONENTS ${QT_COMPONENTS})
    set(QT_LIBS ${QT_LIBS} Qt${QT_VERSION_MAJOR}::${COMPONENTS})
endforeach ()

target_link_libraries(${TEST_NAME}
    PRIVATE
        GTest::gtest
        spdlog::spdlog
        ${QT_LIBS}
)

add_custom_command(TARGET ${TEST_NAME}
    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:spdlog> $<TARGET_FILE_DIR:${TEST_NAME}>
)
