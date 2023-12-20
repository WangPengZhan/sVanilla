

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/googletest/googletest/include
)

add_compile_definitions(TEST_PROJ_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}")
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/googletest test_binary_dir)

option(SQLite_test "ENABLE SQLite test" ON)
if(SQLite_test)
    add_subdirectory(SQLite)
endif()

option(ThreadPool_test "ENABLE TheadPool_test test" ON)
if(ThreadPool_test)
    add_subdirectory(ThreadPool)
endif()

option(Aria2Net_test "ENABLE Aria2Net_test test" ON)
if(Aria2Net_test)
    add_subdirectory(Aria2Net)
endif()

option(NetWork_test "ENABLE NetWork_test test" ON)
if(NetWork_test)
    add_subdirectory(NetWork)
endif()

option(Zipper_test "ENABLE Zipper_test test" ON)
if(Zipper_test)
    add_subdirectory(Zipper)
endif()

option(BiliApi_test "ENABLE BiliApi_test test" ON)
if(BiliApi_test)
    add_subdirectory(BiliApi)
endif()

option(Download_test "ENABLE Download_test test" ON)
if(Download_test)
    add_subdirectory(Download)
endif()

option(FFmpeg_test "ENABLE Download_test test" ON)
if(FFmpeg_test)
    add_subdirectory(FFmpeg)
endif()

