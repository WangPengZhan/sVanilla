#pragma once

#include "Login/login.h"

class BiliLogin : public AbstractLogin
{
public:
    // thread-safe
    LoginSatus getLoginStatus() override;
    bool getScanContext(std::string& content) override;
    void loginSuccess() override;
    UserInfo getUserInfo(std::string dir) override;
    bool logout() override;
    std::vector<Adapter::BaseVideoView> history() override;

    // resource
    const LoginResource& allResources() const override;
    const std::vector<uint8_t>& resource(ResourceIndex index) const override;

    int type() const override;

private:
    std::string m_qrcodeKey;
    static LoginResource m_biliRes;
};
