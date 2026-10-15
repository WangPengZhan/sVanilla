set(TEST_NAME ClientUi_test)

set(QT_COMPONENTS Core)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_COMPONENTS})

# ClientUi 本体是可执行程序（main.cpp + 大量 UI），无法作为库被链接。
# 这里只把其中**无业务依赖的纯逻辑源文件**编进测试。
set(CLIENTUI_SOURCES
    ${CMAKE_SOURCE_DIR}/sVanilla/src/ClientUi/Utils/UrlProcess.cpp
)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS *.h *.cpp)

add_executable(${TEST_NAME} ${TEST_SOURCES} ${CLIENTUI_SOURCES})
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

target_include_directories(${TEST_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/sVanilla/src
)

foreach (COMPONENTS ${QT_COMPONENTS})
    set(QT_LIBS ${QT_LIBS} Qt${QT_VERSION_MAJOR}::${COMPONENTS})
endforeach ()

target_link_libraries(${TEST_NAME}
    PRIVATE
        GTest::gtest_main
        ${QT_LIBS}
)
