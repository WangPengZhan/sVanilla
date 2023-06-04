#pragma once

#include <nlohmann/json.hpp>
#include <vector>


namespace BiliApi
{
// 协议基类
class Protocol
{
public:
    Protocol() = default;
    Protocol(nlohmann::json json) : m_json(std::move(json)) {}
    Protocol(Protocol&& other) noexcept { m_json = std::move(other.m_json); }
    Protocol(const Protocol& other) { m_json = other.m_json; }
    Protocol& operator=(const Protocol& other) { m_json = other.m_json; };
    Protocol& operator=(Protocol&& other) noexcept { m_json = std::move(other.m_json); };

    virtual ~Protocol() = default;
    virtual const std::string toString() { return m_json.dump(); };
    virtual void request() {};

    void SetJson(const nlohmann::json& json) { m_json = json; }
    const nlohmann::json& GetJson() const { return m_json; }

protected:
    nlohmann::json m_json;
};

// 支持json转任意类型
template<typename ProtocolType,
    typename std::enable_if<std::is_convertible<const ProtocolType&, Protocol>::value, int>::type = 0>
void to_json(nlohmann::json& j, const ProtocolType& p) 
{
    j = p.GetJson();
}

template<typename ProtocolType,
    typename std::enable_if<std::is_convertible<const ProtocolType&, Protocol>::value, int>::type = 0>
void from_json(const nlohmann::json& j, ProtocolType& p) 
{
    p.SetJson(j);
}

// BiliApi文档
// https://github.com/SocialSisterYi/bilibili-API-collect

class VideoOwner : public Protocol
{
public:
    void SetMid(long long mid) { m_json["mid"] = mid; }
    const long long GetMid() const { return m_json["mid"]; };

    void SetName(const std::string& name) { m_json["name"] = name; }
    const std::string GetName() const { return m_json["name"]; };

    void SetFace(const std::string& face) { m_json["face"] = face; }
    const std::string GetFace() const { return m_json["face"]; };
};

class VideoStat : public Protocol
{
public:
    void SetAid(long long mid) { m_json["aid"] = mid; }
    const long long GetAid() const { return m_json["aid"]; }

    void SetView(long long view) { m_json["view"] = view; }
    const long long GetView() const { return m_json["view"]; }

    void SetDanmaKu(long long danmaku) { m_json["danmaku"] = danmaku; }
    const long long GetDanmaKu() const { return m_json["danmaku"]; }

    void SetFavorite(long long favorite) { m_json["favorite"] = favorite; }
    const long long GetFavorite() const { return m_json["favorite"]; }

    void SetCoin(long long coin) { m_json["coin"] = coin; }
    const long long GetCoin() const { return m_json["coin"]; }

    void SetShare(long long share) { m_json["share"] = share; }
    const long long GetShare() const { return m_json["share"]; }

    void SetNowRank(long long nowRank) { m_json["now_rank"] = nowRank; }
    const long long GetNowRank() const { return m_json["now_rank"]; }

    void SetHisRank(long long hisRank) { m_json["his_rank"] = hisRank; }
    const long long GetHisRank() const { return m_json["his_rank"]; }

    void SetLike(long long like) { m_json["like"] = like; }
    const long long GetLike() const { return m_json["like"]; }

    void SetDislike(long long dislike) { m_json["dislike"] = dislike; }
    const long long GetDislike() const { return m_json["dislike"]; }

    void SetEvaluation(const std::string& evaluation) { m_json["evaluation"] = evaluation; }
    const std::string GetEvaluation() const { return m_json["evaluation"]; };

    void SetArgueMsg(const std::string& argueMsg) { m_json["argue_msg"] = argueMsg; }
    const std::string GetArgueMsg() const { return m_json["argue_msg"]; };
};


class Dimension : public Protocol
{
public:
    void SetWidth(int width) { m_json["width"] = width; }
    const int GetWidth() const { return m_json["width"]; }

    void SetHeight(int height) { m_json["height"] = height; }
    const int GetHeight() const { return m_json["height"]; }


    void SetRotate(int rotate) { m_json["rotate"] = rotate; }
    const int GetRotate() const { return m_json["rotate"]; }

};

class UgcArc : public Protocol
{
public:
    void SetAid(long long mid) { m_json["aid"] = mid; }
    const long long GetAid() const { return m_json["aid"]; }

    void SetVideos(int videos) { m_json["videos"] = videos; }
    const int GetVideos() const { return m_json["videos"]; }

    void SetTypeId(int typeId) { m_json["type_id"] = typeId; }
    const int GetTypeId() const { return m_json["type_id"]; }

    void SetTypeName(int typeName) { m_json["type_name"] = typeName; }
    const int GetTypeName() const { return m_json["type_name"]; }

    void SetCopyright(int copyright) { m_json["copyright"] = copyright; }
    const int GetCopyright() const { return m_json["copyright"]; }

    void SetPic(const std::string& pic) { m_json["pic"] = pic; }
    const std::string GetPic() const { return m_json["pic"]; }

    void SetTitle(const std::string& title) { m_json["title"] = title; }
    const std::string GetTitle() const { return m_json["title"]; }

    void SetPubdate(long long pubdate) { m_json["pubdate"] = pubdate; }
    const long long GetPubdate() const { return m_json["pubdate"]; }

    void SetCtime(int ctime) { m_json["ctime"] = ctime; }
    const long long GetCtime() const { return m_json["ctime"]; }

    void SetDesc(const std::string& desc) { m_json["desc"] = desc; }
    const std::string GetDesc() const { return m_json["desc"]; }

    void SetState(int state) { m_json["state"] = state; }
    const int GetState() const { return m_json["state"]; }

    void SetDuration(int duration) { m_json["duration"] = duration; }
    const int GetDuration() const { return m_json["duration"]; }

    void SetAuthor(const VideoOwner& author) { m_json["author"] = author; }
    const VideoOwner GetAuthor() const { return m_json["author"]; }

    void SetStat(const VideoStat& stat) { m_json["stat"] = stat; }
    const VideoStat GetStat() const { return m_json["stat"]; }

    void SetDynamic(const std::string& dynamic) { m_json["dynamic"] = dynamic; }
    const std::string GetDynamic() const { return m_json["dynamic"]; }

    void SetDimension(const Dimension& dimension) { m_json["dimension"] = dimension; }
    const Dimension GetDimension() const { return m_json["dimension"]; }
};

class VideoPage : public Protocol
{
public:
    void SetCid(long long cid) { m_json["cid"] = cid; }
    const long long GetCid() const { return m_json["cid"]; }

    void SetPage(int page) { m_json["page"] = page; }
    const int GetPage() const { return m_json["page"]; }


    void SetFrom(const std::string& from) { m_json["from"] = from; }
    const std::string GetFrom() const { return m_json["from"]; }

    void SetPart(const std::string& part) { m_json["part"] = part; }
    const std::string GetPart() const { return m_json["part"]; }

    void SetDuration(int duration) { m_json["duration"] = duration; }
    const int GetDuration() const { return m_json["duration"]; }

    void SetVid(const std::string& vid) { m_json["vid"] = vid; }
    const std::string GetVid() const { return m_json["vid"]; }

    void SetWeblink(const std::string& weblink) { m_json["weblink"] = weblink; }
    const std::string GetWeblink() const { return m_json["weblink"]; }

    void SetDimension(const Dimension& dimension) { m_json["dimension"] = dimension; }
    const Dimension GetDimension() const { return m_json["dimension"]; }


    void SetFirstFrame(const std::string& firstFrame) { m_json["first_frame"] = firstFrame; }
    const std::string GetFirstFrame() const { return m_json["first_frame"]; }

};

class SubtitleAuthor : public Protocol
{
public:
    void SetMid(long long mid) { m_json["mid"] = mid; }
    const long long GetMid() const { return m_json["mid"]; };

    void SetName(const std::string& name) { m_json["name"] = name; }
    const std::string GetName() const { return m_json["name"]; }

    void SetSex(const std::string& sex) { m_json["sex"] = sex; }
    const std::string GetSex() const { return m_json["sex"]; }

    void SetFace(const std::string& face) { m_json["face"] = face; }
    const std::string GetFace() const { return m_json["face"]; }

    void SetSign(const std::string& sign) { m_json["sign"] = sign; }
    const std::string GetSign() const { return m_json["sign"]; }
};

class Subtitle : public Protocol
{
public:
    void SetId(long long id) { m_json["id"] = id; }
    const long long GetId() const { return m_json["id"]; }

    void SetLan(const std::string& lan) { m_json["lan"] = lan; }
    const std::string GetLan() const { return m_json["lan"]; }

    void SetauthorDoc(const std::string& authorDoc) { m_json["author_doc"] = authorDoc; }
    const std::string GetauthorDoc() const { return m_json["author_doc"]; }

    void SetIsLock(bool isLock) { m_json["is_lock"] = isLock; }
    bool GetIsLock() const { return m_json["is_lock"]; }

    void SetAuthorMid(const long long authorMid) { m_json["author_mid"] = authorMid; }
    const long long GetAuthorMid() const { return m_json["author_mid"]; }

    void SetSubtitleUrl(const std::string& subtitleUrl) { m_json["subtitle_url"] = subtitleUrl; }
    const std::string GetSubtitleUrl() const { return m_json["subtitle_url"]; }

    void SetAuthor(const SubtitleAuthor& author) { m_json["author"] = author; }
    const SubtitleAuthor GetAuthor() const { return m_json["author"]; }
};

class VideoSubtitle : public Protocol
{
public:
    void SetAllowSubmit(bool allowSubmit) { m_json["allow_submit"] = allowSubmit; }
    bool GetAllowSubmit() const { return m_json["allow_submit"]; }

    void SetList(const std::list<Subtitle>& list) { m_json["list"] = list; }
    const std::list<Subtitle> GetList() const { return m_json["list"]; }

};

class UgcStat : public Protocol
{
public:
    void SetSeasonId(long long seasonId) { m_json["season_id"] = seasonId; }
    const long long GetSeasonId() const { return m_json["season_id"]; }

    void SetView(long long view) { m_json["view"] = view; }
    const long long GetView() const { return m_json["view"]; }

    void SetDanmaKu(long long danmaku) { m_json["danmaku"] = danmaku; }
    const long long GetDanmaKu() const { return m_json["danmaku"]; }

    void SetRelay(long long relay) { m_json["relay"] = relay; }
    const long long GetRelay() const { return m_json["relay"]; }

    void SetFavorite(long long favorite) { m_json["fav"] = favorite; }
    const long long GetFavorite() const { return m_json["fav"]; }

    void SetCoin(long long coin) { m_json["coin"] = coin; }
    const long long GetCoin() const { return m_json["coin"]; }

    void SetShare(long long share) { m_json["share"] = share; }
    const long long GetShare() const { return m_json["share"]; }

    void SetNowRank(long long nowRank) { m_json["now_rank"] = nowRank; }
    const long long GetNowRank() const { return m_json["now_rank"]; }

    void SetHisRank(long long hisRank) { m_json["his_rank"] = hisRank; }
    const long long GetHisRank() const { return m_json["his_rank"]; }

    void SetLike(long long like) { m_json["like"] = like; }
    const long long GetLike() const { return m_json["like"]; }

};

class UgcEpisode : public Protocol
{
public:
    void SetSeasonId(long long seasonId) { m_json["season_id"] = seasonId; }
    const long long GetSeasonId() const { return m_json["season_id"]; }

    void SetSectionId(long long sectionId) { m_json["section_id"] = sectionId; }
    const long long GetSectionId() const { return m_json["section_id"]; }

    void SetId(long long id) { m_json["id"] = id; }
    const long long GetId() const { return m_json["id"]; }

    void SetAid(long long mid) { m_json["aid"] = mid; }
    const long long GetAid() const { return m_json["aid"]; }

    void SetCid(long long cid) { m_json["cid"] = cid; }
    const long long GetCid() const { return m_json["cid"]; }

    void SetTitle(const std::string& title) { m_json["title"] = title; }
    const std::string GetTitle() const { return m_json["title"]; }

    void SetAttribute(int attribute) { m_json["attribute"] = attribute; }
    const int GetAttribute() const { return m_json["attribute"]; }

    void SetArc(const UgcArc& arc) { m_json["arc"] = arc; }
    const UgcArc GetArc() const { return m_json["arc"]; };

    void SetPage(const VideoPage& page) { m_json["page"] = page; }
    const VideoPage GetPage() const { return m_json["page"]; }

    void SetBvid(const std::string& bvid) { m_json["bvid"] = bvid; }
    const std::string GetBvid() const { return m_json["bvid"]; };
};

class UgcSection : public Protocol
{
public:
    void SetSeasonId(long long seasonId) { m_json["season_id"] = seasonId; }
    const long long GetSeasonId() const { return m_json["season_id"]; }

    void SetId(long long id) { m_json["id"] = id; }
    const long long GetId() const { return m_json["id"]; }

    void SetTitle(const std::string& title) { m_json["title"] = title; }
    const std::string GetTitle() const { return m_json["title"]; }

    void SetAttribute(int attribute) { m_json["attribute"] = attribute; }
    const int GetAttribute() const { return m_json["attribute"]; }

    void SetArc(const UgcArc& arc) { m_json["arc"] = arc; }
    const UgcArc GetArc() const { return m_json["arc"]; };

    void SetPage(const VideoPage& page) { m_json["page"] = page; }
    const VideoPage GetPage() const { return m_json["page"]; }

    void SetType(const std::string& type) { m_json["type"] = type; }
    const std::string GetType() const { return m_json["type"]; }

    void SetEpisodes(const std::list<UgcEpisode>& episodes) { m_json["episodes"] = episodes; }
    const std::list<UgcEpisode> GetEpisodes() const { return m_json["episodes"]; }
};

class UgcSeason : public Protocol
{
public:
    void SetId(long long id) { m_json["id"] = id; }
    const long long GetId() const { return m_json["id"]; }

    void SetTitle(const std::string& title) { m_json["title"] = title; }
    const std::string GetTitle() const { return m_json["title"]; }

    void SetCover(const std::string& cover) { m_json["cover"] = cover; }
    const std::string GetCover() const { return m_json["cover"]; }

    void SetMid(long long mid) { m_json["mid"] = mid; }
    const long long GetMid() const { return m_json["mid"]; }

    void SetIntro(const std::string& intro) { m_json["intro"] = intro; }
    const std::string GetIntro() const { return m_json["intro"]; }

    void SetSignState(int signState) { m_json["sign_state"] = signState; }
    const int GetSignState() const { return m_json["sign_state"]; }

    void SetAttribute(int attribute) { m_json["attribute"] = attribute; }
    const int GetAttribute() const { return m_json["attribute"]; }

    void SetSections(const std::list<UgcSection>& sections) { m_json["sections"] = sections; }
    const std::list<UgcSection> GetSections() const { return m_json["sections"]; }

    void SetStat(const UgcStat& stat) { m_json["stat"] = stat; }
    const UgcStat GetStat() const { return m_json["stat"]; }

    void SetEpCount(int epCount) { m_json["ep_count"] = epCount; }
    const int GetEpCount() const { return m_json["ep_count"]; }

    void SetSeasonType(int seasonType) { m_json["season_type"] = seasonType; }
    const int GetSeasonType() const { return m_json["season_type"]; }

};


class VideoView : public Protocol
{
public:
    void SetBvid(const std::string& bvid) { m_json["bvid"] = bvid; }
    const std::string GetBvid() const { return m_json["bvid"]; };

    void SetAid(long long mid) { m_json["aid"] = mid; }
    const long long GetAid() const { return m_json["aid"]; }

    void SetVideos(int videos) { m_json["videos"] = videos; }
    const int GetVideos() const { return m_json["videos"]; }

    void SetTid(int tid) { m_json["tid"] = tid; }
    const int GetTid() const { return m_json["tid"]; }

    void SetTname(const std::string& tname) { m_json["tname"] = tname; }
    const std::string GetTname() const { return m_json["tname"]; }

    void SetCopyright(int copyright) { m_json["copyright"] = copyright; }
    const int GetCopyright() const { return m_json["copyright"]; }

    void SetPic(const std::string& pic) { m_json["pic"] = pic; }
    const std::string GetPic() const { return m_json["pic"]; }

    void SetTitle(const std::string& title) { m_json["title"] = title; }
    const std::string GetTitle() const { return m_json["title"]; }

    void SetPubdate(long long pubdate) { m_json["pubdate"] = pubdate; }
    const long long GetPubdate() const { return m_json["pubdate"]; }

    void SetCtime(int ctime) { m_json["ctime"] = ctime; }
    const long long GetCtime() const { return m_json["ctime"]; }

    void SetDesc(const std::string& desc) { m_json["desc"] = desc; }
    const std::string GetDesc() const { return m_json["desc"]; }

    void SetState(int state) { m_json["state"] = state; }
    const int GetState() const { return m_json["state"]; }

    void SetDuration(int duration) { m_json["duration"] = duration; }
    const int GetDuration() const { return m_json["duration"]; }

    void SetRedirectUrl(const std::string& redirectUrl) { m_json["redirect_url"] = redirectUrl; }
    const std::string GetRedirectUrl() const { return m_json["redirect_url"]; }

    void SetMissionId(long long missionId) { m_json["mission_id"] = missionId; }
    const long long GetMissionId() const { return m_json["mission_id"]; }

    void SetOwner(const VideoOwner& owner) { m_json["owner"] = owner; }
    const VideoOwner GetOwner() const { return m_json["owner"]; }

    void SetStat(const VideoStat& stat) { m_json["stat"] = stat; }
    const VideoStat GetStat() const { return m_json["stat"]; }

    void SetDynamic(const std::string& dynamic) { m_json["dynamic"] = dynamic; }
    const std::string GetDynamic() const { return m_json["dynamic"]; }

    void SetCid(long long cid) { m_json["cid"] = cid; }
    const long long GetCid() const { return m_json["cid"]; }

    void SetDimension(const Dimension& dimension) { m_json["dimension"] = dimension; }
    const Dimension GetDimension() const { return m_json["dimension"]; }

    void SetSeasonId(long long seasonId) { m_json["season_id"] = seasonId; }
    const long long GetSeasonId() const { return m_json["season_id"]; }

    void SetFestivalJumpUrl(const std::string& festivalJumpUrl) { m_json["festival_jump_url"] = festivalJumpUrl; }
    const std::string GetFestivalJumpUrl() const { return m_json["festival_jump_url"]; }

    void SetPages(const std::list<VideoPage>& pages) { m_json["pages"] = pages; }
    const std::list<VideoPage> GetPages() const { return m_json["pages"]; }

    void SetSubtitle(const VideoSubtitle& subtitle) { m_json["subtitle"] = subtitle; }
    const VideoSubtitle GetSubtitle() const { return m_json["subtitle"]; }

    void SetUgcSeason(const UgcSeason& ugcSeason) { m_json["ugc_season"] = ugcSeason; }
    const UgcSeason GetUgcSeason() const { return m_json["ugc_season"]; }

 
};

class SubtitleInfo : public Protocol
{
public:
    void SetAllowSubmit(bool isAllowSubmit) { m_json["allow_submit"] = isAllowSubmit; }
    bool GetAllowSubmit() const { return m_json["allow_submit"]; }

    void SetLan(const std::string& lan) { m_json["lan"] = lan; }
    const std::string GetLan() const { return m_json["lan"]; }

    void SetLanDoc(const std::string& lanDoc) { m_json["lan_doc"] = lanDoc; }
    const std::string GetLanDoc() const { return m_json["lan_doc"]; }

    void SetSubtitles(const std::list<Subtitle>& subtitles) { m_json["subtitles"] = subtitles; }
    const std::list<Subtitle> GetSubtitles() const { return m_json["subtitles"]; }
};

class PlayerV2 : public Protocol
{
public:
    void SetAid(long long mid) { m_json["aid"] = mid; }
    const long long GetAid() const { return m_json["aid"]; }

    void SetBvid(const std::string& bvid) { m_json["bvid"] = bvid; }
    const std::string GetBvid() const { return m_json["bvid"]; }

    void SetCid(long long cid) { m_json["cid"] = cid; }
    const long long GetCid() const { return m_json["cid"]; }

    void SetSubtitle(const SubtitleInfo& subtitleInfo) { m_json["subtitle"] = subtitleInfo; }
    const SubtitleInfo GetSubtitle() const { return m_json["subtitle"]; }
};

class PlayUrlDurl : public Protocol
{
public:
    void SetOrder(int order) { m_json["order"] = order; }
    const int GetOrder() const { return m_json["order"]; }

    void SetLength(const std::string& length) { m_json["length"] = length; }
    const std::string& GetLength() const { return m_json["length"]; }

    void SetSize(long long size) { m_json["size"] = size; }
    const long long GetSize() const { return m_json["size"]; }

    void SetUrl(const std::string& url) { m_json["url"] = url; }
    const std::string GetUrl() const { return m_json["url"]; }

    void SetBackupUrl(const std::list<std::string>& backupUrl) { m_json["backup_url"] = backupUrl; }
    const std::list<std::string>& GetBackupUrl() const { return m_json["backup_url"]; }
};

class PlayUrlDashVideo : public Protocol
{
public:
    void SetId(long long id) { m_json["id"] = id; }
    const long long GetId() const { return m_json["id"]; }

    void SetBaseUrl(const std::string& baseUrl) { m_json["base_url"] = baseUrl; }
    const std::string GetBaseUrl() const { return m_json["base_url"]; }

    void SetBackupUrl(const std::list<std::string>& backupUrl) { m_json["backup_url"] = backupUrl; }
    const std::list<std::string>& GetBackupUrl() const { return m_json["backup_url"]; }

    void SetMimeType(const std::string& mimeType) { m_json["mimeType"] = mimeType; }
    const std::string GetMimeType() const { return m_json["mimeType"]; }

    void SetCodecs(const std::string& codecs) { m_json["codecs"] = codecs; }
    const std::string GetCodecs() const { return m_json["codecs"]; }

    void SetWidth(int width) { m_json["width"] = width; }
    const int GetWidth() const { return m_json["width"]; }

    void SetHeight(int height) { m_json["height"] = height; }
    const int GetHeight() const { return m_json["height"]; }

    void SetFrameRate(const std::string& frameRate) { m_json["frameRate"] = frameRate; }
    const std::string GetFrameRate() const { return m_json["frameRate"]; }

};

class PlayUrlDashDolby : public Protocol
{
public:
    void SetAudio(const std::list<PlayUrlDashVideo>& audio) { m_json["audio"] = audio; }
    const std::list<PlayUrlDashVideo> GetAudio() const { return m_json["audio"]; }
};

class PlayUrlDash : public Protocol
{
public:
    void SetVideo(const std::list<PlayUrlDashVideo>& video) { m_json["video"] = video; }
    const std::list<PlayUrlDashVideo> GetVideo() const { return m_json["video"]; }

    void SetAudio(const std::list<PlayUrlDashVideo>& audio) { m_json["audio"] = audio; }
    const std::list<PlayUrlDashVideo> GetAudio() const { return m_json["audio"]; }

    void SetDolby(const PlayUrlDashDolby& dolby) { m_json["dolby"] = dolby; }
    const PlayUrlDashDolby GetDolby() const { return m_json["dolby"]; }
};


class PlayUrlSupportFormat : public Protocol
{
public:
    void SetQuality(int quality) { m_json["quality"] = quality; }
    const int GetQuality() const { return m_json["quality"]; }

    void SetFormat(const std::string& format) { m_json["format"] = format; }
    const std::string GetFormat() const { return m_json["format"]; }

    void SetNewDescription(const std::string& newDescription) { m_json["new_description"] = newDescription; }
    const std::string GetNewDescription() const { return m_json["new_description"]; }

    void SetSuperscript(const std::string& superscript) { m_json["superscript"] = superscript; }
    const std::string GetSuperscript() const { return m_json["superscript"]; }
}; 

class PlayUrl : public Protocol
{
public:
    void SetAcceptDescription(const std::list<std::string>& acceptDescription) { m_json["accept_description"] = acceptDescription; }
    const std::list<std::string>& GetAcceptDescription() const { return m_json["accept_description"]; }

    void SetAcceptQuality(const std::list<int>& acceptQuality) { m_json["accept_quality"] = acceptQuality; }
    const std::list<int>& GetAcceptQuality() const { return m_json["accept_quality"]; }

    void SetDurl(const std::list<PlayUrlDurl>& durl) { m_json["durl"] = durl; }
    const std::list<PlayUrlDurl>& GetDurl() const { return m_json["durl"]; }

    void SetDash(PlayUrlDash dash) { m_json["dash"] = dash; }
    const PlayUrlDash GetDash() const { return m_json["dash"]; }

    void SetSupportFormats(const std::list<PlayUrlSupportFormat>& supportFormats) { m_json["support_formats"] = supportFormats; }
    const std::list<PlayUrlSupportFormat> GetSupportFormats() const { return m_json["support_formats"]; }
};

// https://passport.bilibili.com/qrcode/getLoginUrl
class LoginUrl : public Protocol 
{
public:
    void SetOauthKey(const std::string& oauthKey)
    {
        m_json["oauthKey"] = oauthKey;
    }
    const std::string GetOauthKey() const
    {
        return m_json["oauthKey"];
    }
    void SetUrl(const std::string& url)
    {
        m_json["url"] = url;
    }
    const std::string GetUrl() const
    {
        return m_json["url"];
    }
};

class LoginUrlOrigin : public Protocol
{
public:
    LoginUrl GetData() const
    {
        return m_json["data"];
    }
    //void SetData(const LoginUrl& data)
    //{
    //    m_json["data"] = data;
    //}
    void SetStatus(bool status)
    {
        m_json["status"] = status;
    }
    bool GetStatus() const
    {
        return m_json["status"];
    }
};


// https://passport.bilibili.com/qrcode/getLoginInfo
class LoginStatusData : public Protocol 
{
public:
    void SetUrl(const std::string& url)
    {
        m_json["url"] = url;
    }
    const std::string GetUrl() const
    {
        return m_json["url"];
    }
};

class LoginStatusScanning : public Protocol 
{
public:
    LoginStatusData GetData() const
    {
        return m_json["data"];
    }
    //void SetData(const LoginStatusData& data)
    //{
    //     m_json["data"] = data;
    // }
    void SetStatus(bool status)
    {
        m_json["status"] = status;
    }
    bool GetStatus() const
    {
        return m_json["status"];
    }
    void SetMessage(const std::string& message)
    {
        m_json["message"] = message;
    }
    const std::string GetMessage() const
    {
        return m_json["message"];
    }
};

class LoginStatus : public Protocol 
{
public:
    int GetCode() const
    {
        return m_json["code"];
    }
    void SetData(int code)
    {
         m_json["code"] = code;
    }
    void SetStatus(bool status)
    {
        m_json["status"] = status;
    }
    bool GetStatus() const
    {
        return m_json["status"];
    }
    void SetMessage(const std::string& message)
    {
        m_json["message"] = message;
    }
    const std::string GetMessage() const
    {
        return m_json["message"];
    }

};

class LoginStatusReady : public Protocol 
{
public:
    int GetCode() const
    {
        return m_json["code"];
    }
    void SetData(int code)
    {
        m_json["code"] = code;
    }
    LoginStatusData GetData() const
    {
        return m_json["data"];
    }
    // void SetData(const LoginStatusData& data)
    //{
    //      m_json["data"] = data;
    //  }
    void SetStatus(bool status)
    {
        m_json["status"] = status;
    }
    bool GetStatus() const
    {
        return m_json["status"];
    }
};

} // namespace BiliApi




