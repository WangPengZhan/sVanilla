#include "BilibiliVideoView.h"
#include <sstream>
#include <QDebug>

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

Adapter::VideoView ConvertVideoView(const biliapi::VideoView& data)
{
    Adapter::VideoView videoListView;
    if (checkSeason(data))
    {
        const auto episodes = data.ugc_season.sections.front().episodes;
        for (const auto& e : episodes)
        {
            Adapter::BaseVideoView v = ConvertEpisodes(e);
            v.Publisher = data.owner.name;
            videoListView.push_back(v);
        }
        return videoListView;
    }
    else if (checkPages(data))
    {
        const auto pages = data.pages;
        for (const auto& p : pages)
        {
            Adapter::BaseVideoView v = ConvertPages(p);
            v.Identifier = data.bvid;
            v.Description = data.desc;
            v.Publisher = data.owner.name;
            videoListView.push_back(v);
        }
        return videoListView;
    }
    videoListView.push_back(ConvertSingleVideo(data));
    return videoListView;
}

bool checkSeason(const biliapi::VideoView& data)
{
    if (data.ugc_season.sections.empty())
    {
        return false;
    }
    return !data.ugc_season.sections.front().episodes.empty();
}

bool checkPages(const biliapi::VideoView& data)
{
    return data.pages.size() > 1;
}

Adapter::BaseVideoView ConvertEpisodes(const biliapi::UgcEpisode& data)
{
    return Adapter::BaseVideoView{
        .Identifier = data.bvid,
        .AlternateId = std::to_string(data.aid),
        .VideoId = std::to_string(data.cid),
        .Title = data.title,
        .Cover = data.arc.pic,
        .Duration = formatDuration(data.page.duration),
        .Description = data.arc.desc,
    };
}

Adapter::BaseVideoView ConvertPages(const biliapi::VideoPage& data)
{
    return Adapter::BaseVideoView{
        //        .Identifier = data.bvid,
        .AlternateId = std::to_string(data.cid),   .VideoId = std::to_string(data.cid), .Title = data.part, .Cover = data.first_frame,
        .Duration = formatDuration(data.duration),
        //        .Description = data.desc,
    };
}

Adapter::BaseVideoView ConvertSingleVideo(const biliapi::VideoView& data)
{
    return Adapter::BaseVideoView{
        .Identifier = data.bvid,
        .AlternateId = std::to_string(data.aid),
        .VideoId = std::to_string(data.cid),
        .Title = data.title,
        .Publisher = data.owner.name,
        .Cover = data.pic,
        .Duration = formatDuration(data.duration),
        .Description = data.desc,
    };
}
