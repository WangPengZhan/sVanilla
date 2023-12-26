#pragma once
#include <string>
#include <future>
#include <functional>

namespace ffmpeg
{

struct MergeInfo
{
    std::string audio;
    std::string video;
    std::string targetVideo;
};

class FFmpegHelper
{
public:
    FFmpegHelper();
    ~FFmpegHelper();

    static void megerVideo(const MergeInfo& mergeInfo);
    static void megerVideo(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc);
    static FFmpegHelper& globalInstance();

    void startFFpmegAsync(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc);
    void closeFFmpeg();

private:
    std::list<std::future<bool>> m_futures;
};

}  // namespace ffmpeg
