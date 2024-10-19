#include <gmock/gmock.h>

#include <Aria2Net/AriaClient/AriaClient.h>

#include <QDebug>

using namespace testing;
using namespace aria2net;

class Aria2RemoteTesting : public Test
{
public:
    AriaClient& ariaClient = AriaClient::globalClient();

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(Aria2RemoteTesting, GetAriaVersion)
{
    // auto const res = ariaClient.Call<AriaVersion>("getVersion", {});
    // qDebug() << res.result.version.c_str();
    // ASSERT_THAT(res.result.version, Eq("1.36.0"));
}

TEST_F(Aria2RemoteTesting, AddSingleUri)
{
    // std::list<std::string> uris = {"https://filesamples.com/samples/video/mp4/sample_1280x720_surfing_with_audio.mp4"};
    ////    nlohmann::json params = uris;
    // AriaOption option;
    // auto const res = ariaClient.Call<AriaAddUri>("addUri", {uris, option, 1});
    // qDebug() << res.result.c_str();
    // ASSERT_THAT(res.result, Not(IsEmpty()));
}

TEST_F(Aria2RemoteTesting, AddMultipleUris)
{
    // std::list<std::string> uris = {"https://filesamples.com/samples/video/mp4/sample_1280x720_surfing_with_audio.mp4",
    //                                "https://filesamples.com/samples/video/3gp/sample_1280x720_surfing_with_audio.3gp"};
    // AriaOption option;
    // auto const res = ariaClient.Call<AriaAddUri>("addUri", {uris, option, 1});
    // qDebug() << res.result.c_str();
    // ASSERT_THAT(res.result, Not(IsEmpty()));
}

TEST_F(Aria2RemoteTesting, GetGlobalOptionAsync)
{
    // auto const res = ariaClient.Call<AriaGetOption>("getGlobalOption", {});
    // qDebug() << res.result.toString().c_str();
    // ASSERT_THAT(res.result.toString(), Not(IsEmpty()));
}