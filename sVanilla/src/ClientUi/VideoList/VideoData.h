#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include <QListWidgetItem>

namespace Adapter
{
struct BaseVideoView;
}  //  namespace Adapter

struct DownloadConfig;

struct VideoInfoFull
{
    std::shared_ptr<DownloadConfig> downloadConfig;
    std::shared_ptr<Adapter::BaseVideoView> videoView;

    std::string getGuid() const;
    std::unordered_map<std::string, std::string> nameRules() const;
    std::string fileName() const;
    std::string coverPath() const;

    static const std::vector<std::string> ruleList;
};

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
