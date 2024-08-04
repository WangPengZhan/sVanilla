#include "BiliLogin.h"

#include "BiliResource.h"
#include "BiliApi/BilibiliClient.h"
#include "ClientUi/Utils/CoverUtil.h"
#include "Util/TimerUtil.h"
#include "ClientUi/Adapter/BaseVideoView.h"

namespace
{

Adapter::BaseVideoView convertHistory(const biliapi::HistoryInfo& data)
{
    auto item = Adapter::BaseVideoView();
    item.Identifier = data.history.bvid;
    item.AlternateId = std::to_string(data.history.cid);
    item.VideoId = std::to_string(data.history.cid);
    item.Title = data.title;
    item.Cover = data.cover.empty() ? (data.covers.empty() ? "" : data.covers.front()) : data.cover;
    // item.Cover = data.cover;
    item.Duration = formatDuration(data.duration);
    item.Description = data.new_desc;
    item.PublishDate = convertTimestamp(data.view_at);

    return item;
}

std::vector<Adapter::BaseVideoView> convertVideoView(const biliapi::History& data)
{
    std::vector<Adapter::BaseVideoView> videoListView;
    videoListView.reserve(256);

    for (const auto& historyInfo : data.data.list)
    {
        videoListView.emplace_back(convertHistory(historyInfo));
    }

    return videoListView;
}
}  // namespace

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

UserInfo BiliLogin::getUserInfo(std::string dir)
{
    UserInfo userInfo;

    const auto nav = biliapi::BilibiliClient::globalClient().getNavInfo();
    if (nav.code != 0)
    {
        return userInfo;
    }
    userInfo.uname = nav.data.uname;

    if (!nav.data.face.empty())
    {
        std::string path = dir + "/" + std::to_string(nav.data.mid) + ".jpg";
        FILE* file = fopen(path.c_str(), "wb");
        network::NetWork netWork;
        biliapi::BilibiliClient::globalClient().get(nav.data.face, file);
        fclose(file);
        userInfo.facePath = path;
    }

    return userInfo;
}

bool BiliLogin::logout()
{
    const auto logoutInfo = biliapi::BilibiliClient::globalClient().getLogoutExitV2();
    return logoutInfo.code == 0;
}

std::vector<Adapter::BaseVideoView> BiliLogin::history()
{
    biliapi::HistoryQueryParam param;
    param.ps = 30;
    const auto historyInfo = biliapi::BilibiliClient::globalClient().getHistory(param);

    return convertVideoView(historyInfo);
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

int BiliLogin::type() const
{
    return 1;
}
