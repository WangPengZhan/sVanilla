#include "VideoData.h"
#include "Adapter/BaseVideoView.h"
#include "ClientUi/Config/SingleConfig.h"
#include "Util/UrlProcess.h"

std::string VideoInfoFull::getGuid() const
{
    auto guid = videoView->Identifier + videoView->AlternateId + videoView->VideoId + downloadConfig->downloadDir.toStdString() +
                std::to_string(static_cast<int>(downloadConfig->videoQuality)) + downloadConfig->nameRule.toStdString();

    return guid;
}

std::unordered_map<std::string, std::string> VideoInfoFull::nameRules() const
{
    std::unordered_map<std::string, std::string> result;
    if (!videoView)
    {
        return result;
    }

    result.insert({ruleList[0], videoView->Identifier});
    result.insert({ruleList[1], videoView->Title});
    result.insert({ruleList[2], videoView->Publisher});
    result.insert({ruleList[3], videoView->PublishDate});
    return result;
}

std::string VideoInfoFull::fileName() const
{
    std::string result;
    if (!downloadConfig || !videoView)
    {
        return result;
    }

    QString temp = downloadConfig->nameRule;
    QString subRule = "$" + QString::fromStdString(ruleList[0]) + "$";
    temp.replace(subRule, QString::fromStdString(videoView->Identifier));
    subRule = "$" + QString::fromStdString(ruleList[1]) + "$";
    temp.replace(subRule, QString::fromStdString(videoView->Title));
    subRule = "$" + QString::fromStdString(ruleList[2]) + "$";
    temp.replace(subRule, QString::fromStdString(videoView->Publisher));
    subRule = "$" + QString::fromStdString(ruleList[3]) + "$";
    temp.replace(subRule, QString::fromStdString(videoView->PublishDate));

    if (temp.isEmpty())
    {
        temp = QString::fromStdString(videoView->Title);
    }
    temp = util::FileHelp::removeSpecialChar(temp);

    return temp.toStdString();
}

const std::vector<std::string> VideoInfoFull::ruleList = {"id", "title", "auther", "data"};

VideoListWidgetItem::VideoListWidgetItem(std::shared_ptr<VideoInfoFull> videoInfo, int index)
    : QListWidgetItem()
    , m_index(index)
    , m_videoInfo(std::move(videoInfo))
{
}

bool VideoListWidgetItem::operator<(const QListWidgetItem& other) const
{
    auto videoWidgetItem = static_cast<const VideoListWidgetItem&>(other);
    switch (m_orderType)
    {
    case OrderType::Origin:
        return m_index < videoWidgetItem.m_index;
    case OrderType::Title:
        return m_videoInfo->videoView->Title < videoWidgetItem.m_videoInfo->videoView->Title;
    case OrderType::Date:
        return m_videoInfo->videoView->PublishDate < videoWidgetItem.m_videoInfo->videoView->PublishDate;
    case OrderType::Duration:
    {
        if (m_videoInfo->videoView->Duration.size() == videoWidgetItem.m_videoInfo->videoView->Duration.size())
        {
            return m_videoInfo->videoView->Duration < videoWidgetItem.m_videoInfo->videoView->Duration;
        }
        else
        {
            return m_videoInfo->videoView->Duration.size() < videoWidgetItem.m_videoInfo->videoView->Duration.size();
        }
    }
    default:
        break;
    }

    return true;
}

OrderType VideoListWidgetItem::orderType() const
{
    return m_orderType;
}

void VideoListWidgetItem::setOrderType(OrderType orderType)
{
    m_orderType = orderType;
}
