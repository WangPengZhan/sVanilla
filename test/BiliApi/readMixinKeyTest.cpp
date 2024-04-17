
#include <gmock/gmock.h>
#include "BiliApi/BiliApiConstants.h"
#include <BiliApi/BilibiliClient.h>
#include "BiliApi/BilibiliUtils.h"
#include "spdlog/async.h"
#include "spdlog/sinks/rotating_file_sink.h"

#include <QDebug>

using namespace testing;
using namespace biliapi;

class ReadMixinKeyTesting : public Test
{
public:
    BilibiliClient& client = BilibiliClient::globalClient();

    void SetUp() override
    {
        spdlog::rotating_logger_mt<spdlog::async_factory>("Network", "log/Network.log", 1024 * 1024 * 10, 100);
        // spdlog::rotating_logger_mt<spdlog::async_factory>("Aria2Net", "log/Aria2Net.log", 1024 * 1024 * 10, 100);
    }
};

TEST_F(ReadMixinKeyTesting, ReadMixinKey)
{
    nlohmann::json j = readJson("sVanilla.data");
    if (!j.is_object() || !j.contains("mixinKey"))
    {
        qDebug() << "mixinKey not found";
    }
    nlohmann::json cookie = j["mixinKey"];
    if (!cookie.is_object())
    {
        qDebug() << "mixinKey is not a object";
    }

    if (cookie.contains("Expires") && isExpired(cookie["Expires"].get<std::time_t>()))
    {
        qDebug() << "mixinKey is expired";
    }

    const auto mixinKey = MixinKey(cookie);
    qDebug() << "mixin" << QString::fromStdString(mixinKey.mixin_key);
    ASSERT_THAT(mixinKey.mixin_key, Not(IsEmpty()));
}