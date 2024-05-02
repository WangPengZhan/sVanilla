#include "VideoData.h"
#include "Adapter/BaseVideoView.h"

std::string VideoInfoFull::getGuid() const
{
    if (guid.empty())
    {
        guid = videoView->Identifier + videoView->AlternateId + videoView->VideoId;
    }

    return guid;
}