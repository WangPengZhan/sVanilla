#include "BilibiliVideoView.h"
#include <sstream>

std::string formatDuration(const int duration) {
    std::chrono::seconds sec(duration);
    const auto hours = std::chrono::duration_cast<std::chrono::hours>(sec);
    sec -= std::chrono::duration_cast<std::chrono::seconds>(hours);
    const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(sec);
    sec -= std::chrono::duration_cast<std::chrono::seconds>(minutes);

    std::ostringstream formattedDuration;
    if(hours.count() > 0) {
        formattedDuration << std::setfill('0') << std::setw(2) << hours.count() << ":";
    }
    formattedDuration << std::setfill('0') << std::setw(2) << minutes.count() << ":"
                      << std::setfill('0') << std::setw(2) << sec.count();

    return formattedDuration.str();
}


Adapter::VideoView ConvertToVideoView(const BiliApi::VideoView& data)
{
    Adapter::VideoView videoView;
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