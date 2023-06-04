#pragma once

#include <string>

#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkReply>

struct curl_slist;

class CNetWork
{
public:
    enum HpptType
    {
        GET,
        POST,
        PUT,
    };

    using ParamType = std::unordered_map<std::string, std::string>;

public:
    CNetWork();
    ~CNetWork();

    void HttpGet(const std::string& url, ParamType params, std::string& response);
    void HttpGet(const std::string& url, const std::string& params, std::string& response);
    void HttpGet(const std::string& url, std::string& response);

    void HttpPost(const std::string& url, ParamType params, std::string& response);
    void HttpPost(const std::string& url, const std::string& params, std::string& response);
    void HttpPost(const std::string& url, std::string& response);

    void HttpPut(const std::string& url, ParamType params, std::string& response);
    void HttpPut(const std::string& url, std::string& response);

    void SetHeaders(curl_slist* headers);
    void AppendHeaders(curl_slist* headers);
    void AppendHeaders(const std::string& header);

protected:
    void InitDefaultHeaders();
    void InitDefaultHeadersLogin();

    curl_slist* m_headers; // 请求头
    class CurlHelp
    {
    public:
        CurlHelp();
        ~CurlHelp();
    };

    static CurlHelp m_curlHelp;
};

class AriaClient : public QThread
{
    Q_OBJECT
public:
    enum ResponseType
    {
        Async,
        Sync
    };

    AriaClient(QObject* parent = nullptr);
    ~AriaClient();

    QByteArray Request(const QString& url, const QString& parameters, int retry = 3, ResponseType responseType = Sync);
    inline QByteArray Request(const std::string& url, const std::string& parameters, int retry = 3, ResponseType responseType = Sync)
    {
        return Request(QString::fromLocal8Bit(url.data()), QString::fromLocal8Bit(parameters.data()), retry, responseType);
    }

    QByteArray Request(const std::string& parameters, int retry = 3);

    QByteArray RequestAsync(const QString& url, const QString& parameters, int retry = 3);
    inline QByteArray RequestAsync(const std::string& url, const std::string& parameters, int retry = 3)
    {
        return RequestAsync(QString::fromLocal8Bit(url.data()), QString::fromLocal8Bit(parameters.data()), retry);
    }

    QByteArray RequestSync(const QString& url, const QString& parameters, int retry = 3);
    inline QByteArray RequestSync(const std::string& url, const std::string& parameters, int retry = 3)
    {
        return RequestSync(QString::fromLocal8Bit(url.data()), QString::fromLocal8Bit(parameters.data()), retry);
    }


    static QByteArray Response(QNetworkReply* reply);

    void SetDataStream(QDataStream& dataStream);

protected:
    void run() override;
    QNetworkAccessManager* m_syncNetworkAccessManager;
    QNetworkAccessManager* m_asyncNetworkAccessManager;
};
