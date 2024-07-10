#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

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

    static const std::vector<std::string> ruleList;
};

struct InfoPanelData
{
    int previousRow;
    int currentRow;
    const std::shared_ptr<VideoInfoFull>& info;
};
