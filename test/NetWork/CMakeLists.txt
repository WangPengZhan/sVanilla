set(NetWork_test NetWork_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)
file(GLOB_RECURSE TESTED_SOURCES LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/NetWork/*.h ${CMAKE_CURRENT_SOURCE_DIR}/../../sVanilla/src/NetWork/*.cpp)
set(SOURCES ${TEST_SOURCES} ${TESTED_SOURCES})

set(CMAKE_AUTOMOC ON)

find_package(spdlog REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(CURL REQUIRED)



add_executable(${NetWork_test} ${SOURCES})
add_test(NAME ${NetWork_test} COMMAND ${NetWork_test})
add_compile_definitions(TEST_PROJ_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(${NetWork_test}
    PRIVATE
        GTest::gtest_main
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        CURL::libcurl
)
