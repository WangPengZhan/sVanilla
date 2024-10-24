#pragma once
#include <QObject>

class UriProcess : public QObject
{
    Q_OBJECT
public:
    struct UriInfo
    {
        std::string id;
        std::string type;
    };

    explicit UriProcess(QObject* parent = nullptr);

    static bool isUri(const std::string& uri);
    void parseUri(const std::string& uri);
    void updateWebsiteIcon(const std::string& uri);
    std::string checkUriType(const std::string& uri);

signals:
    void uriProcessComplete(const UriProcess::UriInfo& uriInfo);
    void setWebsiteIcon(const QString& iconPath);

private:
    std::string m_lastUri;
    bool m_isTimeOut = true;
};
