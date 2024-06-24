#pragma once

#include "Login/login.h"

class BiliLogin : public AbstractLogin
{
public:
    // thread-safe
    LoginSatus getLoginStatus() override;
    bool getScanContext(std::string& content) override;
    void loginSuccess() override;

    // resource
    const LoginResource& allResources() const override;
    const std::vector<uint8_t>& resource(ResourceIndex index) const override;

private:
    std::string m_qrcodeKey;
    static LoginResource m_biliRes;
};
