#include "BilibiliVideoView.h"
#include <sstream>
#include <QDebug>

std::string convertTimestamp(long long timestamp)
{
    std::chrono::time_point<std::chrono::system_clock> tp = std::chrono::system_clock::from_time_t(static_cast<std::time_t>(timestamp));
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    std::tm* t = std::gmtime(&tt);
    std::ostringstream oss;
    oss << std::put_time(t, "%F");
    return oss.str();
}

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

Adapter::Views ConvertVideoView(const biliapi::VideoView& data)
{
    Adapter::Views videoListView;

    if (checkSeason(data))
    {
        const auto episodes = data.ugc_season.sections.front().episodes;
        for (const auto& e : episodes)
        {
            const auto item = ConvertEpisodes(e);
            item->Publisher = data.owner.name;
            videoListView.push_back(item);
        }
        return videoListView;
    }
    else if (checkPages(data))
    {
        const auto pages = data.pages;
        for (const auto& p : pages)
        {
            const auto item = ConvertPages(p);
            item->Identifier = data.bvid;
            videoListView.push_back(item);
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

std::shared_ptr<Adapter::BaseVideoView> ConvertEpisodes(const biliapi::UgcEpisode& data)
{
    auto item = std::make_shared<Adapter::BaseVideoView>();
    item->Identifier = data.bvid;
    item->AlternateId = std::to_string(data.aid);
    item->VideoId = std::to_string(data.cid);
    item->Title = data.title;
    item->Cover = data.arc.pic;
    item->Duration = formatDuration(data.page.duration);
    item->Description = data.arc.desc.empty() ? data.arc.desc : data.page.part;
    item->PublishDate = convertTimestamp(data.arc.pubdate);
    return item;
}
std::shared_ptr<Adapter::BaseVideoView> ConvertPages(const biliapi::VideoPage& data)
{
    auto item = std::make_shared<Adapter::BaseVideoView>();
    item->AlternateId = std::to_string(data.cid);
    item->VideoId = std::to_string(data.cid);
    item->Title = data.part;
    item->Cover = data.first_frame;
    item->Duration = formatDuration(data.duration);
    return item;
}
std::shared_ptr<Adapter::BaseVideoView> ConvertSingleVideo(const biliapi::VideoView& data)
{
    auto item = std::make_shared<Adapter::BaseVideoView>();
    item->Identifier = data.bvid;
    item->AlternateId = std::to_string(data.aid);
    item->VideoId = std::to_string(data.cid);
    item->Title = data.title;
    item->Publisher = data.owner.name;
    item->Cover = data.pic;
    item->Duration = formatDuration(data.duration);
    item->Description = data.desc;
    item->PublishDate = convertTimestamp(data.pubdate);
    return item;
}