#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include <QListWidgetItem>

#include <BaseVideoView.h>

namespace adapter
{
struct BaseVideoView;
}  // namespace adapter

struct DownloadConfig;

constexpr char coverDir[] = "cover";

struct InfoPanelData
{
    int previousRow;
    int currentRow;
    const std::shared_ptr<VideoInfoFull>& info;
};

enum class OrderType
{
    Origin,
    Title,
    Date,
    Duration
};

class VideoListWidgetItem : public QListWidgetItem
{
public:
    VideoListWidgetItem(std::shared_ptr<VideoInfoFull> videoInfo, int index);

    bool operator<(const QListWidgetItem& other) const override;

    OrderType orderType() const;
    void setOrderType(OrderType orderType);

private:
    OrderType m_orderType = OrderType::Origin;
    int m_index{-1};
    std::shared_ptr<VideoInfoFull> m_videoInfo;
};
