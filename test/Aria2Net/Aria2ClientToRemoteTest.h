#include "Aria2Net/Protocol/Protocol.h"
#include "NetWork/CNetWork.h"
#include <Aria2Net/AriaClient/AriaClient.h>
#include <QUuid>

std::string GetRpcUri(int listenPort = 6800)
{
    return QString("http://10.0.2.122:%1/jsonrpc").arg(listenPort).toStdString();
}

class Aria2RemoteTest : public CNetWork
{
public:
    Aria2RemoteTest() = default;

    ~Aria2RemoteTest() = default;

    Aria2RemoteTest(const Aria2RemoteTest& other) = delete;

    Aria2RemoteTest& operator=(const Aria2RemoteTest& other) = delete;

    static Aria2RemoteTest& globalClient()
    {
        static Aria2RemoteTest client;
        return client;
    };
    using ListString = std::list<std::string>;

    aria2net::AriaSendData PrepareAriaSendData()
    {
        aria2net::AriaSendData ariaSend;

        QUuid id = QUuid::createUuid();
        ariaSend.id = id.toString().toStdString();

        ariaSend.jsonrpc = JSONRPC;

        ariaSend.method = "aria2.getVersion";

        ListString listString;
        listString.emplace_back(std::string("token:") + TOKEN);
        ariaSend.params = listString;

        return ariaSend;
    };

    aria2net::AriaVersion GetAriaVersionAsync()
    {
        auto ariaSend = PrepareAriaSendData();

        return GetResult<aria2net::AriaVersion>(ariaSend);
    };

    std::string Request(const std::string& url, const std::string& params)
    {
        std::string strResponse;
        HttpPost(url, params, strResponse);
        return strResponse;
    };

private:
    static constexpr char TOKEN[] = "password";
    static constexpr char JSONRPC[] = "2.0";

    template <typename Result> Result GetResult(const aria2net::AriaSendData& sendData)
    {
        std::string strParams = sendData.toString();
        std::string strResponse = Request(GetRpcUri(), strParams);
        nlohmann::json result = nlohmann::json::parse(strResponse);
        return Result(result);
    };
};