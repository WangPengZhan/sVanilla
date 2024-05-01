#pragma once
#include <memory>
#include <string>

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

private:
    mutable std::string guid;
};