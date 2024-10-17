set(BiliApi_test BiliApi_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)
file(GLOB_RECURSE TESTED_SOURCES LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/Aria2Net/*.h ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/Aria2Net/*.cpp)
file(GLOB_RECURSE TESTED_DEPANDED_SOURCES LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/NetWork/*.h ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/NetWork/*.cpp
                                                                 ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/Util/*.h ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/Util/*.cpp)

file(GLOB_RECURSE WBI LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/BiliApi/*.h ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/BiliApi/*.cpp)

set(SOURCES ${TEST_SOURCES} ${TESTED_SOURCES} ${TESTED_DEPANDED_SOURCES} ${WBI})

set(CMAKE_AUTOMOC ON)
# qt
set(QT_COMPONENTS
    Core
    Gui
    Widgets
    Network
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_COMPONENTS} LinguistTools)
find_package(spdlog REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(CURL REQUIRED)

# 寻找OpenSSL库
find_package(OpenSSL REQUIRED)

# 如果OpenSSL被找到，包含OpenSSL的头文件目录
if(OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
endif()

add_executable(${BiliApi_test} ${SOURCES})
add_test(NAME ${BiliApi_test} COMMAND ${BiliApi_test})
add_compile_definitions(TEST_PROJ_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}")

foreach (COMPONENTS ${QT_COMPONENTS})
    set(QT_LIBS ${QT_LIBS} Qt${QT_VERSION_MAJOR}::${COMPONENTS})
endforeach ()
target_link_libraries(${BiliApi_test}
    PRIVATE
        GTest::gtest 
        GTest::gmock 
        GTest::gtest_main 
        GTest::gmock_main
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        CURL::libcurl
        ${QT_LIBS}
        ${OPENSSL_LIBRARIES}
)
