#pragma once
#include <array>
#include <string>
#include <vector>

struct UserInfo
{
    std::string facePath;
    std::string uname;
    std::string vipType;
};

namespace Adapter
{

struct BaseVideoView;

}  // namespace Adapter

class AbstractLogin
{
public:
    enum LoginSatus
    {
        Unknow,
        Error,
        NoScan,
        Timeout,
        ScanedNoAck,
        Success
    };

    enum ResourceIndex
    {
        Background,
        Loading,
        Tip,
        WaitConfirm,
        Complete,
        Confirmed,
        Refresh
    };

    using LoginResource = std::array<std::vector<uint8_t>, 7>;

    virtual bool isSupportScanQrc() const
    {
        return true;
    }

    // thread-safe
    virtual LoginSatus getLoginStatus() = 0;
    virtual bool getScanContext(std::string& content) = 0;
    virtual void loginSuccess() = 0;
    virtual UserInfo getUserInfo(std::string dir) = 0;
    virtual bool logout() = 0;
    virtual std::vector<Adapter::BaseVideoView> history() = 0;

    // resource
    virtual const LoginResource& allResources() const = 0;
    virtual const std::vector<uint8_t>& resource(ResourceIndex index) const = 0;

    // type
    virtual int type() const = 0;

protected:
    std::vector<uint8_t> m_emptyString;
};
