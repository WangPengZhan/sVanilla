set(TEST_NAME BaseQt_test)
set(TESTED_LIB BaseQt)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS *.h *.cpp)

set(QT_COMPONENTS
    Core
    Gui
    Widgets
    Svg
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_COMPONENTS})

set(CMAKE_AUTOMOC ON)

add_executable(${TEST_NAME} ${TEST_SOURCES})
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

# QFontMetrics / QPixmap 需要 QGuiApplication：测试自带 main() 并切换到 offscreen 平台。
set_tests_properties(${TEST_NAME} PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

target_include_directories(${TEST_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/sVanilla/src
)

foreach (COMPONENTS ${QT_COMPONENTS})
    set(QT_LIBS ${QT_LIBS} Qt${QT_VERSION_MAJOR}::${COMPONENTS})
endforeach ()

# 注意：用 GTest::gtest（不含 main），main() 由本测试提供。
target_link_libraries(${TEST_NAME}
    PRIVATE
        ${TESTED_LIB}
        GTest::gtest
        spdlog::spdlog
        ${QT_LIBS}
)

add_custom_command(TARGET ${TEST_NAME}
    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:spdlog> $<TARGET_FILE_DIR:${TEST_NAME}>
)
