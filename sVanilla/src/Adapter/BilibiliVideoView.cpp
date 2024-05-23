#include "BilibiliVideoView.h"
#include "Util/TimerUtil.h"

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