
#include <gmock/gmock.h>
#include <Aria2ClientToRemoteTest.h>

using namespace testing;




TEST(GetRpcUri, GetRpcUri)
{
    EXPECT_EQ(GetRpcUri(), "http://10.0.2.122:6800/jsonrpc");
}



class Aria2RemoteTesting : public Test
{
public:
    Aria2RemoteTest& m_aria2RemoteTest = Aria2RemoteTest::globalClient();
};

TEST_F(Aria2RemoteTesting, PrepareAriaSendData)
{
    auto ariaSendData = m_aria2RemoteTest.PrepareAriaSendData();
    ASSERT_THAT(ariaSendData.id, Not(IsEmpty()));

    std::list<std::string> paramList = {"token:password"};
    ASSERT_THAT(ariaSendData.params, Eq(paramList));
}

TEST_F(Aria2RemoteTesting, GetAriaVersion)
{
    auto GetVersionResult = m_aria2RemoteTest.GetAriaVersionAsync();
    ASSERT_THAT(GetVersionResult.result.version, Eq("1.36.0"));
}

TEST_F(Aria2RemoteTesting, Request)
{
    std::string strResponse = m_aria2RemoteTest.Request(GetRpcUri(), "{\"jsonrpc\":\"2.0\",\"id\":\"c4a0e3a3-7e8a-4e4e-8e8a-4e4e7e8a4e4e\",\"method\":\"aria2.getVersion\",\"params\":[\"token:password\"]}");
    ASSERT_THAT(strResponse, Eq("{\"id\":\"c4a0e3a3-7e8a-4e4e-8e8a-4e4e7e8a4e4e\",\"jsonrpc\":\"2.0\",\"result\":{\"enabledFeatures\":[\"Async DNS\",\"BitTorrent\",\"Firefox3 Cookie\",\"GZip\",\"HTTPS\",\"Message Digest\",\"Metalink\",\"XML-RPC\",\"libuv\"],\"version\":\"1.36.0\"}}"));
}