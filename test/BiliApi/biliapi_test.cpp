#include <vector>
#include <chrono>
#include <thread>
#include <map>
#include <any>
#include <fstream>

#include <gtest/gtest.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>

#include <nlohmann/json.hpp>

#include "Aria2Net/AriaServer/AriaServer.h"
#include "Aria2Net/Protocol/Protocol.h"

TEST(Aria2Net_Test, STARTSERVER)
{
    spdlog::rotating_logger_mt<spdlog::async_factory>("Aria2Net", "log/Aria2Net.log", 1024 * 1024 * 10, 100);

    aria2net::AriaServer server;
    server.startLocalServerAsync();
}

TEST(Aria2Net_Test, EN)
{
    using namespace aria2net;

    AriaError ariaError;
    ariaError.code = 1;
    ariaError.message = "test error";

    AriaUri ariaUri;
    ariaUri.status = "status";
    ariaUri.uri = "https://www.baidu.com";

    AriaAddMetalink addMetaLink;
    addMetaLink.id = "id";
    addMetaLink.jsonrpc = "jsonrpc";
    addMetaLink.result = "addMetaLink";
    addMetaLink.error = ariaError;

    AriaAddTorrent addTorrent;
    addTorrent.id = "id";
    addTorrent.jsonrpc = "jsonrpc";
    addTorrent.result = "addTorrent";
    addTorrent.error = ariaError;

    AriaAddUri addUri;
    addUri.id = "id";
    addUri.jsonrpc = "jsonrpc";
    addUri.result = "addUri";
    addUri.error = ariaError;

    AriaChangeUri changeUri;
    changeUri.id = "id";
    changeUri.jsonrpc = "jsonrpc";
    changeUri.result = {1, 2, 3, 4, 5};
    changeUri.error = ariaError;

    AriaGetFiles getFiles;
    getFiles.id = "id";
    getFiles.jsonrpc = "jsonrpc";
    getFiles.result = {ariaUri};
    getFiles.error = ariaError;
}
