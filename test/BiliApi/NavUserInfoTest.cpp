#include <gmock/gmock.h>
#include "BiliApi/BiliApiConstants.h"
#include <BiliApi/BilibiliClient.h>
#include "BiliApi/BilibiliUtils.h"
#include "spdlog/async.h"
#include "spdlog/sinks/rotating_file_sink.h"

#include <QDebug>

using namespace testing;
using namespace biliapi;

class NavUserInfoTesting : public Test
{
public:
    BilibiliClient& client = BilibiliClient::globalClient();

    void SetUp() override
    {
        spdlog::rotating_logger_mt<spdlog::async_factory>("Network", "log/Network.log", 1024 * 1024 * 10, 100);
        // spdlog::rotating_logger_mt<spdlog::async_factory>("Aria2Net", "log/Aria2Net.log", 1024 * 1024 * 10, 100);
    }
};

TEST_F(NavUserInfoTesting, GetUserInfo)
{
    std::string response;
    client.Rquest(CNetWork::GET, PassportURL::WebNav, {}, response, client.getDefalutHeaders(), true);
    qDebug() << QString::fromStdString(response);
    ASSERT_THAT(response, Not(IsEmpty()));
}