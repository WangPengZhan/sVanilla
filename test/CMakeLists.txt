
enable_testing()
include(GoogleTest)

add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/utf-8>)

add_compile_definitions(TEST_PROJ_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}")
find_package(GTest CONFIG REQUIRED)

option(Base_test "ENABLE Base test" ON)
if(Base_test)
    add_subdirectory(Base)
endif()

option(Logger_test "ENABLE Logger test" ON)
if(Logger_test)
    add_subdirectory(Logger)
endif()

option(BaseQt_test "ENABLE BaseQt test" ON)
if(BaseQt_test)
    add_subdirectory(BaseQt)
endif()

option(SQLite_test "ENABLE SQLite test" ON)
if(SQLite_test)
    add_subdirectory(SQLite)
endif()

option(ThreadPool_test "ENABLE TheadPool_test test" ON)
if(ThreadPool_test)
    add_subdirectory(ThreadPool)
endif()

option(Zipper_test "ENABLE Zipper_test test" ON)
if(Zipper_test)
    add_subdirectory(Zipper)
endif()

option(Download_test "ENABLE Download test" ON)
if(Download_test)
    add_subdirectory(Download)
endif()

option(Plugin_test "ENABLE Plugin test" ON)
if(Plugin_test)
    add_subdirectory(Plugin)
endif()

option(Aria2Net_test "ENABLE Aria2Net test" ON)
if(Aria2Net_test)
    add_subdirectory(Aria2Net)
endif()

option(SUI_test "ENABLE SUI test" ON)
if(SUI_test)
    add_subdirectory(SUI)
endif()

option(ClientUi_test "ENABLE ClientUi test" ON)
if(ClientUi_test)
    add_subdirectory(ClientUi)
endif()

