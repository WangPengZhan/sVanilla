#include "VideoData.h"

#include <BaseVideoView.h>

#include "ClientUi/Config/SingleConfig.h"
#include "Utils/UrlProcess.h"

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
