cmake_minimum_required(VERSION 3.15)

project(Logger 
    VERSION 1.0.0 
    LANGUAGES CXX)

find_package(spdlog REQUIRED)
find_package(unofficial-breakpad CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES "*.cpp" "*.h")

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        spdlog::spdlog
        unofficial::breakpad::libbreakpad
        unofficial::breakpad::libbreakpad_client
)
