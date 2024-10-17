set(Aria2Net_test Aria2Net_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)
file(GLOB_RECURSE TESTED_SOURCES LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/Aria2Net/*.h ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/Aria2Net/*.cpp)
file(GLOB_RECURSE TESTED_DEPANDED_SOURCES LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/NetWork/*.h ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/NetWork/*.cpp
                                                                ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/BiliApi/*.h ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/BiliApi/*.cpp
                                                                ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/Util/*.h ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/Util/*.cpp
                                                                ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/FFmpeg/*.h ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/FFmpeg/*.cpp)
set(SOURCES ${TEST_SOURCES} ${TESTED_SOURCES} ${TESTED_DEPANDED_SOURCES})

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

add_executable(${Aria2Net_test} ${SOURCES})
#add_test(NAME ${Aria2Net_test} COMMAND ${Aria2Net_test})
#add_compile_definitions(TEST_PROJ_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}")
#add_dependencies(${Aria2Net_test} gtest gmock gtest_main gmock_main)

gtest_discover_tests(${Aria2Net_test}
        # set a working directory so your project root so that you can find test data via paths relative to the project root
        WORKING_DIRECTORY ${PROJECT_DIR}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
)

foreach (COMPONENTS ${QT_COMPONENTS})
    set(QT_LIBS ${QT_LIBS} Qt${QT_VERSION_MAJOR}::${COMPONENTS})
endforeach ()
target_link_libraries(${Aria2Net_test}
    PRIVATE
        GTest::gtest 
        GTest::gmock 
        GTest::gtest_main 
        GTest::gmock_main
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        CURL::libcurl
        ${QT_LIBS}
)

# build event
add_custom_command(TARGET ${Aria2Net_test}
    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory  ${CMAKE_CURRENT_SOURCE_DIR}/../../tools $<TARGET_FILE_DIR:${PROJECT_NAME}>
)