#include "BilibiliVideoView.h"

Adapter::VideoView ConvertToVideoView(const BiliApi::VideoView& data)
{
    Adapter::VideoView videoView;
    videoView.Identifier = data.bvid;
    videoView.AlternateId = std::to_string(data.aid);
    videoView.VideoId = std::to_string(data.cid);
    videoView.Title = data.title;
    videoView.Duration = data.duration;
    videoView.Publisher = data.owner.name;
    return videoView;
}