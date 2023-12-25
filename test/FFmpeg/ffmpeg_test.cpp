#include <vector>
#include <chrono>
#include <thread>
#include <map>
#include <any>

#include <gtest/gtest.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>

#include "Aria2Net/AriaServer/AriaServer.h"

TEST(Aria2Net_Test, STARTSERVER)
{
    spdlog::rotating_logger_mt<spdlog::async_factory>("Aria2Net", "log/Aria2Net.log", 1024 * 1024 * 10, 100);

    aria2net::AriaServer server;
    server.startServerAsync();
}

TEST(Aria2Net_Test, EN)
{
}
