#include "BiliLogin.h"

#include "BiliResource.h"
#include "BiliApi/BilibiliClient.h"

BiliLogin::LoginResource BiliLogin::m_biliRes{qrc_background, qrc_loading, qrc_tip, qrc_waitConfirm, qrc_complete, qrc_init, qrc_refresh};

BiliLogin::LoginSatus BiliLogin::getLoginStatus()
{
    const auto loginStatus = biliapi::BilibiliClient::globalClient().getLoginStatus(m_qrcodeKey);

    if (loginStatus.code == 0)
    {
        if (loginStatus.data.code == 86038)
        {
            return Timeout;
        }
        else if (loginStatus.data.code == 86101)
        {
            return NoScan;
        }
        else if (loginStatus.data.code == 86090)
        {
            return ScanedNoAck;
        }
        else if (loginStatus.data.code == 0)
        {
            return Success;
        }
        else
        {
            return Error;
        }
    }
    else
    {
        return Error;
    }
}

bool BiliLogin::getScanContext(std::string& content)
{
    const auto login = biliapi::BilibiliClient::globalClient().getLoginUrl();
    if (login.data.qrcode_key.empty() || login.code != 0)
    {
        return false;
    }

    m_qrcodeKey = login.data.qrcode_key;
    content = login.data.url;
    return true;
}

void BiliLogin::loginSuccess()
{
}

const BiliLogin::LoginResource& BiliLogin::allResources() const
{
    return m_biliRes;
}

const std::vector<uint8_t>& BiliLogin::resource(ResourceIndex index) const
{
    if (index >= 0 && index < m_biliRes.size())
    {
        return m_biliRes.at(index);
    }

    return m_emptyString;
}
