#include "VideoData.h"
#include "Adapter/BaseVideoView.h"
#include "ClientUi/Config/SingleConfig.h"
#include "Util/UrlProcess.h"

std::string VideoInfoFull::getGuid() const
{
    if (guid.empty())
    {
        guid = videoView->Identifier + videoView->AlternateId + videoView->VideoId + downloadConfig->downloadDir.toStdString() +
               std::to_string(static_cast<int>(downloadConfig->videoQuality)) + downloadConfig->nameRule.toStdString();
    }

    return guid;
}

std::unordered_map<std::string, std::string> VideoInfoFull::nameRules() const
{
    std::unordered_map<std::string, std::string> result;
    if (!videoView)
    {
        return result;
    }

    result.insert({ruleList[0], videoView->Identifier});
    result.insert({ruleList[1], videoView->Title});
    result.insert({ruleList[2], videoView->Publisher});
    result.insert({ruleList[3], videoView->PublishDate});
    return result;
}

std::string VideoInfoFull::fileName() const
{
    std::string result;
    if (!downloadConfig || !videoView)
    {
        return result;
    }

    QString subRule = "$" + QString::fromStdString(ruleList[0]) + "$";
    auto temp = downloadConfig->nameRule.replace(subRule, QString::fromStdString(videoView->Identifier));
    subRule = "$" + QString::fromStdString(ruleList[1]) + "$";
    temp = downloadConfig->nameRule.replace(subRule, QString::fromStdString(videoView->Title));
    subRule = "$" + QString::fromStdString(ruleList[2]) + "$";
    temp = downloadConfig->nameRule.replace(subRule, QString::fromStdString(videoView->Publisher));
    subRule = "$" + QString::fromStdString(ruleList[3]) + "$";
    temp = downloadConfig->nameRule.replace(subRule, QString::fromStdString(videoView->PublishDate));

    if (temp.isEmpty())
    {
        temp = QString::fromStdString(videoView->Title);
    }
    temp = util::FileHelp::removeSpecialChar(temp);

    return temp.toStdString();
}

const std::vector<std::string> VideoInfoFull::ruleList = {"id", "title", "auther", "data"};
