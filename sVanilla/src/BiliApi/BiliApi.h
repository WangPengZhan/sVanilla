#pragma once

#include <nlohmann/json.hpp>
#include <list>

namespace biliapi
{
// BiliApi文档
// https://github.com/SocialSisterYi/bilibili-API-collect

class VideoOwner
{
public:
    long long mid;
    std::string name;
    std::string face;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(VideoOwner, mid, name, face)
};

class ArgueInfo
{
public:
    std::string argue_link;
    std::string argue_msg;
    int argue_type;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ArgueInfo, argue_link, argue_msg, argue_type)
};

class VideoStat
{
public:
    long long aid;
    long long view;
    long long danmaku;
    long long favorite;
    long long coin;
    long long share;
    long long now_rank;
    long long his_rank;
    long long like;
    long long dislike;
    std::string evaluation;
    ArgueInfo argue_info;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(VideoStat, aid, view, danmaku, favorite, coin, share, now_rank, his_rank, like, dislike, evaluation, argue_info)
};

class Dimension
{
public:
    int width;
    int height;
    int rotate;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Dimension, width, height, rotate)
};

class UgcArc
{
public:
    long long aid;
    // int videos;
    // int type_id;
    // int type_name;
    // int copyright;
    std::string pic;
    std::string title;
    long long pubdate;
    // int ctime;
    std::string desc;
    // int state;
    int duration;
    // VideoOwner author;
    // VideoStat stat;
    // std::string dynamic;
    // Dimension dimension;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(UgcArc, aid, pic, title, pubdate, desc, duration)
};

class VideoPage
{
public:
    long long cid;
    // int page;
    std::string first_frame;
    std::string part;
    int duration;
    Dimension dimension;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(VideoPage, cid, first_frame, part, duration, dimension)
};

class SubtitleAuthor
{
public:
    long long mid;
    std::string name;
    std::string sex;
    std::string face;
    std::string sign;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(SubtitleAuthor, mid, name, sex, face, sign)
};

class Subtitle
{
public:
    long long id;
    std::string lan;
    std::string author_doc;
    bool is_lock;
    long long author_mid;
    std::string subtitle_url;
    SubtitleAuthor author;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Subtitle, id, lan, author_doc, is_lock, author_mid, subtitle_url, author)
};

class VideoSubtitle
{
public:
    bool allow_submit;
    std::list<Subtitle> list;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(VideoSubtitle, allow_submit, list)
};

class UgcStat
{
public:
    long long season_id;
    long long view;
    long long danmaku;
    long long relay;
    long long favorite;
    long long coin;
    long long share;
    long long nowRank;
    long long hisRank;
    long long like;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(UgcStat, season_id, view, danmaku, relay, favorite, coin, share, nowRank, hisRank, like)
};

class UgcEpisode
{
public:
    // long long season_id;
    // long long section_id;
    // long long id;
    // long long mid;
    long long aid;
    long long cid;
    std::string title;
    // int attribute;
    UgcArc arc;
    VideoPage page;
    std::string bvid;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(UgcEpisode, aid, cid, title, arc, page, bvid)
};

class UgcSection
{
public:
    // long long season_id;
    // long long id;
    std::string title;
    // int attribute;
    // UgcArc arc;
    // VideoPage page;
    // std::string type;
    std::list<UgcEpisode> episodes;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(UgcSection, title, episodes)
};

class UgcSeason
{
public:
    long long id;
    std::string title;
    // std::string cover;
    // long long mid;
    std::string intro;
    // int signState;
    // int attribute;
    std::list<UgcSection> sections;
    // UgcStat stat;
    int ep_count;
    // int season_type;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(UgcSeason, id, title, intro, sections, ep_count)
};

class VideoView
{
public:
    std::string bvid;
    long long aid;
    int tid;
    std::string tname;
    std::string pic;
    std::string title;
    long long pubdate;
    std::string desc;
    int duration;
    VideoOwner owner;
    std::string dynamic;
    long long cid;
    std::list<VideoPage> pages;
    // VideoSubtitle subtitle;
    UgcSeason ugc_season;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(VideoView, bvid, aid, tid, tname, pic, title, pubdate, desc, duration, owner, dynamic, cid, pages, ugc_season)
};

class VideoViewOrigin
{
public:
    VideoView data;
    int code{};

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(VideoViewOrigin, data, code)
};

class SubtitleInfo
{
public:
    bool allow_submit;
    std::string lan;
    std::string lan_doc;
    std::list<Subtitle> subtitles;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(SubtitleInfo, allow_submit, lan, lan_doc, subtitles)
};

class PlayerV2
{
public:
    long long aid;
    std::string bvid;
    long long cid;
    SubtitleInfo subtitle;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PlayerV2, aid, bvid, cid, subtitle)
};

class PlayUrlDurl
{
public:
    int order;
    std::string length;
    long long size;
    std::string url;
    std::list<std::string> backup_url;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PlayUrlDurl, order, length, size, backup_url)
};

class PlayUrlDashVideo
{
public:
    long long id;
    std::string base_url;
    std::list<std::string> backup_url;
    std::string mimeType;
    std::string codecs;
    int width;
    int height;
    std::string frameRate;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PlayUrlDashVideo, id, base_url, backup_url, mimeType, codecs, width, height, frameRate)
};

class PlayUrlDashDolby
{
public:
    std::list<PlayUrlDashVideo> audio;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PlayUrlDashDolby, audio)
};

class PlayUrlDash
{
public:
    std::list<PlayUrlDashVideo> video;
    std::list<PlayUrlDashVideo> audio;
    PlayUrlDashDolby dolby;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PlayUrlDash, video, audio, dolby)
};

class PlayUrlSupportFormat
{
public:
    int quality;
    std::string format;
    std::string new_description;
    std::string superscript;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PlayUrlSupportFormat, quality, format, new_description, superscript)
};

class PlayUrl
{
public:
    std::list<std::string> accept_description;
    std::list<int> accept_quality;
    std::list<PlayUrlDurl> durl;
    PlayUrlDash dash;
    std::string accept_format;
    std::list<PlayUrlSupportFormat> support_formats;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PlayUrl, accept_description, accept_quality, durl, dash, accept_format, support_formats)
};

class Durl
{
public:
    int order;
    int length;
    int size;
    std::string ahead;
    std::string vhead;
    std::string url;
    std::list<std::string> backup_url;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Durl, order, length, size, ahead, vhead, url, backup_url)
};

class SupportFormats
{
public:
    int quality;
    std::string format;
    std::string new_description;
    std::string display_desc;
    std::string superscript;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(SupportFormats, quality, format, new_description, display_desc, superscript)
};

class PlayUrlData
{
public:
    std::string from;
    std::string result;
    std::string message;
    int quality;
    std::string format;
    int timelength;
    std::string accept_format;
    std::list<std::string> accept_description;
    std::list<int> accept_quality;
    int video_codecid;
    std::string seek_param;
    std::string seek_type;
    std::list<Durl> durl;
    std::list<SupportFormats> support_formats;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PlayUrlData, from, result, message, quality, format, timelength, accept_format, accept_description,
                                                accept_quality, video_codecid, seek_param, seek_type, durl, support_formats)
};

class PlayUrlOrigin
{
public:
    PlayUrlData data;
    int code{};
    int ttl;
    std::string message;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PlayUrlOrigin, data, code, ttl, message)
};

class LoginUrl
{
public:
    std::string qrcode_key;
    std::string url;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(LoginUrl, qrcode_key, url)
};

class LoginUrlOrigin
{
public:
    LoginUrl data;
    int code;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(LoginUrlOrigin, data, code)
};

class LoginStatusData
{
public:
    std::string url;
    std::string refresh_token;
    std::string message;
    int code;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(LoginStatusData, url, refresh_token, message, code)
};

class LoginStatusScanning
{
public:
    LoginStatusData data;
    int code{};
    std::string message;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(LoginStatusScanning, data, code, message)
};

class WbiImg
{
public:
    std::string img_url;
    std::string sub_url;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(WbiImg, img_url, sub_url)
};

class NavData
{
public:
    bool isLogin;
    int email_verified;
    std::string face;
    int mid;
    int mobile_verified;
    int money;
    int moral;
    std::string uname;
    int vipType;
    WbiImg wbi_img;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(NavData, isLogin, email_verified, face, mid, mobile_verified, money, moral, uname, vipType, wbi_img)
};

class Nav
{
public:
    NavData data;
    int code{};
    std::string message;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Nav, data, code, message)
};

class LoginStatus
{
public:
    int code;
    bool status;
    std::string message;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(LoginStatus, code, status, message)
};

class LoginStatusReady
{
public:
    int code;
    LoginStatusData data;
    bool status;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(LoginStatusReady, code, status, data)
};

class RedirectUrl
{
public:
    std::string redirectUrl;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(RedirectUrl, redirectUrl)
};

class LogoutExitV2
{
public:
    int code;
    bool status;
    uint64_t ts;
    std::string message;
    RedirectUrl data;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(LogoutExitV2, code, status, ts, message, data)
};

class Cookie
{
public:
    std::time_t Expires;
    std::string SESSDATA;
    std::string bili_jct;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Cookie, Expires, SESSDATA, bili_jct)
};

class MixinKey
{
public:
    std::time_t Expires;
    std::string mixin_key;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(MixinKey, Expires, mixin_key)
};

class HistoryCursor
{
public:
    int max;
    int view_at;
    std::string business;
    int ps;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(HistoryCursor, max, view_at, business, ps)
};

class HistoryTab
{
public:
    std::string type;
    std::string name;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(HistoryTab, type, name)
};

class HistoryDetailInfo
{
public:
    int oid;
    int epid;
    std::string bvid;
    int page;
    int cid;
    std::string part;
    std::string business;
    int dt;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(HistoryDetailInfo, oid, epid, bvid, page, cid, part, business, dt)
};

class HistoryInfo
{
public:
    std::string title;
    std::string long_title;
    std::string cover;
    std::vector<std::string> covers;
    std::string uri;
    HistoryDetailInfo history;
    int videos;
    std::string author_name;
    std::string author_face;
    int author_mid;
    uint64_t view_at;
    int progress;
    std::string badge;
    int duration;
    int total;
    std::string new_desc;
    int is_finish;
    int is_fav;
    int kid;
    std::string tag_name;
    int live_status;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(HistoryInfo, title, long_title, cover, uri, history, videos, author_name, author_face, author_mid, view_at,
                                                progress, badge, duration, total, new_desc, is_finish, is_fav, kid, tag_name, live_status)
};

class HistoryQueryInfo
{
public:
    HistoryCursor cursor;
    std::vector<HistoryTab> tab;
    std::vector<HistoryInfo> list;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(HistoryQueryInfo, cursor, tab, list)
};

class History
{
public:
    int code;
    std::string message;
    int ttl;
    HistoryQueryInfo data;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(History, code, message, ttl, data)
};

}  // namespace biliapi
