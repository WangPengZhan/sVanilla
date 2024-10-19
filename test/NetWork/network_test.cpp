#include <vector>
#include <chrono>
#include <thread>
#include <map>
#include <any>

#include <gtest/gtest.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>

#include "NetWork/CNetWork.h"

TEST(Network_Test, HttpGet)
{
    spdlog::rotating_logger_mt<spdlog::async_factory>("Aria2Net", "log/Aria2Net.log", 1024 * 1024 * 10, 100);

    network::NetWork netWork;
    std::string response;
    netWork.get("https://www.baidu.com", response);

    EXPECT_EQ(response.empty(), false);
}

TEST(Network_Test, HttpGetParam)
{
}
