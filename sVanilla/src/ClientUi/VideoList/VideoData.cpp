#include "VideoData.h"
#include "Adapter/BaseVideoView.h"
#include "ClientUi/Config/SingleConfig.h"

std::string VideoInfoFull::getGuid() const
{
    if (guid.empty())
    {
        guid = videoView->Identifier + videoView->AlternateId + videoView->VideoId + downloadConfig->downloadDir.toStdString() +
               std::to_string(static_cast<int>(downloadConfig->videoQuality)) + downloadConfig->nameRule.toStdString();
    }

    return guid;
}