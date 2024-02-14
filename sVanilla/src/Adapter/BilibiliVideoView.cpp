#include "BilibiliVideoView.h"
#include <sstream>

std::string formatDuration(const int duration)
{
    std::chrono::seconds sec(duration);
    const auto hours = std::chrono::duration_cast<std::chrono::hours>(sec);
    sec -= std::chrono::duration_cast<std::chrono::seconds>(hours);
    const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(sec);
    sec -= std::chrono::duration_cast<std::chrono::seconds>(minutes);

    std::ostringstream formattedDuration;
    if (hours.count() > 0)
    {
        formattedDuration << std::setfill('0') << std::setw(2) << hours.count() << ":";
    }
    formattedDuration << std::setfill('0') << std::setw(2) << minutes.count() << ":" << std::setfill('0') << std::setw(2) << sec.count();

    return formattedDuration.str();
}

Adapter::BaseVideoView ConvertToVideoView(const BiliApi::VideoView& data)
{
    Adapter::BaseVideoView videoView;
    videoView.Identifier = data.bvid;
    videoView.AlternateId = std::to_string(data.aid);
    videoView.VideoId = std::to_string(data.cid);
    videoView.Title = data.title;
    videoView.Duration = formatDuration(data.duration);
    videoView.Publisher = data.owner.name;
    videoView.Cover = data.pic;
    videoView.Description = data.desc;
    return videoView;
}

std::vector<Adapter::BaseVideoView> ConvertToVideoListView(const BiliApi::VideoView& data)
{
    std::vector<Adapter::BaseVideoView> videoListView;
    const auto episodes = data.ugc_season.sections.front().episodes;
    for (const auto& e : episodes)
    {
        Adapter::BaseVideoView videoView;
        videoView.Identifier = e.bvid;
        videoView.AlternateId = std::to_string(e.aid);
        videoView.VideoId = std::to_string(e.cid);
        videoView.Title = e.title;
        videoView.Duration = formatDuration(e.page.duration);
        videoView.Publisher = data.owner.name;
        videoView.Cover = e.arc.pic;
        videoView.Description = e.arc.desc;

        videoListView.push_back(videoView);
    }
    return videoListView;
}
bool checkSeason(const BiliApi::VideoView& data)
{
    if (data.ugc_season.sections.empty())
    {
        return false;
    }
    return !data.ugc_season.sections.front().episodes.empty();
}
Adapter::VideoView ConvertVideoView(const BiliApi::VideoView& data)
{
    Adapter::VideoView videoListView;
    if (checkSeason(data))
    {
        const auto episodes = data.ugc_season.sections.front().episodes;
        for (const auto& e : episodes)
        {
            Adapter::BaseVideoView videoView;
            videoView.Identifier = e.bvid;
            videoView.AlternateId = std::to_string(e.aid);
            videoView.VideoId = std::to_string(e.cid);
            videoView.Title = e.title;
            videoView.Duration = formatDuration(e.page.duration);
            videoView.Publisher = data.owner.name;
            videoView.Cover = e.arc.pic;
            videoView.Description = e.arc.desc;

            videoListView.push_back(videoView);
        }
    }
    else
    {
        Adapter::BaseVideoView videoView;
        videoView.Identifier = data.bvid;
        videoView.AlternateId = std::to_string(data.aid);
        videoView.VideoId = std::to_string(data.cid);
        videoView.Title = data.title;
        videoView.Duration = formatDuration(data.duration);
        videoView.Publisher = data.owner.name;
        videoView.Cover = data.pic;
        videoView.Description = data.desc;

        videoListView.push_back(videoView);
    }
    return videoListView;
}