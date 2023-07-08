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
    virtual ~Protocol() = default;
    virtual std::string toString() = 0;
};

// // 支持json转任意类型
// template<typename ProtocolType,
//     typename std::enable_if<std::is_convertible<const ProtocolType&, Protocol>::value, int>::type = 0>
// void to_json(nlohmann::json& j, const ProtocolType& p) 
// {
//     j = p.GetJson();
// }

// template<typename ProtocolType,
//     typename std::enable_if<std::is_convertible<const ProtocolType&, Protocol>::value, int>::type = 0>
// void from_json(const nlohmann::json& j, ProtocolType& p) 
// {
//     p.SetJson(j);
// }

// BiliApi文档
// https://github.com/SocialSisterYi/bilibili-API-collect

class VideoOwner : public Protocol
{
public:
    long long mid;
    std::string name;
    std::string face;

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(VideoOwner, mid, name, face)
};

class VideoStat : public Protocol
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
    std::string argue_msg;

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(VideoStat, aid, view, danmaku, favorite,\
        coin, share, now_rank, his_rank, like, dislike, evaluation, argue_msg)
};


class Dimension : public Protocol
{
public:
    int width;
    int height;
    int rotate;

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Dimension, width, height, rotate)
};

class UgcArc : public Protocol
{
public:
    long long aid;
    int videos;
    int type_id;
    int type_name;
    int copyright;
    std::string pic;
    std::string title;
    long long pubdate;
    int ctime;
    std::string desc;
    int state;
    int duration;
    VideoOwner author;
    VideoStat stat;
    std::string dynamic;
    Dimension dimension;

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(UgcArc, aid, videos, type_id, type_name, copyright,\
        pic, title, pubdate, ctime, desc, state, duration, author, stat, dynamic, dimension)

};

class VideoPage : public Protocol
{
public:
    long long cid;
    int page;
    std::string from;
    std::string part;
    int duration;
    std::string vid;
    std::string weblink;
    Dimension dimension;
    std::string first_frame;

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(VideoPage, cid, page, from, part, duration, vid,\
        weblink, dimension, first_frame)
};

class SubtitleAuthor : public Protocol
{
public:
    long long mid;
    std::string name;
    std::string sex;
    std::string face;
    std::string sign;

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SubtitleAuthor, mid, name, sex, face, sign)
};

class Subtitle : public Protocol
{
public:
    long long id;
    std::string lan;
    std::string author_doc;
    bool is_lock;
    long long author_mid;
    std::string subtitle_url;
    SubtitleAuthor author;

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Subtitle, id, lan, author_doc, is_lock, author_mid,\
        subtitle_url, author)

};

class VideoSubtitle : public Protocol
{
public:
    bool allow_submit;
    std::list<Subtitle> list;

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(VideoSubtitle, allow_submit, list)
};

class UgcStat : public Protocol
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

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(UgcStat, season_id, view, danmaku, relay, \
        favorite, coin, share, nowRank, hisRank, like)
};

class UgcEpisode : public Protocol
{
public:
    long long season_id;
    long long section_id;
    long long id;
    long long mid;
    long long cid;
    std::string title;
    int attribute;
    UgcArc arc;
    VideoPage page;
    std::string bvid;

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(UgcEpisode, season_id, section_id, id, mid, \
        cid, title, attribute, arc, page, bvid)
};

class UgcSection : public Protocol
{
public:
    long long season_id;
    long long id;
    std::string title;
    int attribute;
    UgcArc arc;
    VideoPage page;
    std::string type;
    std::list<UgcEpisode> episodes;

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(UgcSection, season_id, id, title, attribute, \
        arc, page, type, arc, episodes)
};

class UgcSeason : public Protocol
{
public:
    long long id;
    std::string title;
    std::string cover;
    long long mid;
    std::string intro;
    int signState;
    int attribute;
    UgcSection sections;
    UgcStat stat;
    int ep_count;
    int season_type;

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(UgcSeason, id, title, cover, mid, \
        intro, signState, attribute, sections, stat, ep_count, season_type)

};


class VideoView : public Protocol
{
public:
    std::string bvid;
    long long aid;
    int videos;
    int tid;
    std::string tname;
    int copyright;
    std::string pic;
    std::string title;
    long long pubdate;
    int ctime;
    std::string desc;
    int state;
    int duration;
    std::string redirect_url;
    long long mission_id;
    VideoOwner owener;
    VideoStat stat;
    std::string dynamic;
    long long cid;
    Dimension dimension;
    long long season_id;
    std::string festival_jump_url;
    std::list<VideoPage> pages;
    VideoSubtitle subtitle;
    UgcSeason ugc_season;

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(VideoView, bvid, aid, videos, tid, tname, copyright, pic, title,\
        pubdate, ctime, desc, state, duration, redirect_url, mission_id, owener, stat, dynamic, cid,\
        dimension, season_id, festival_jump_url, pages, subtitle, ugc_season)
 
};

class SubtitleInfo : public Protocol
{
public:
    bool allow_submit;
    std::string lan;
    std::string lan_doc;
    std::list<Subtitle> subtitles;

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SubtitleInfo, allow_submit, lan, lan_doc, subtitles)
 
};

class PlayerV2 : public Protocol
{
public:
    long long aid;
    std::string bvid;
    long long cid;
    SubtitleInfo subtitle;

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(PlayerV2, aid, bvid, cid, subtitle)

};

class PlayUrlDurl : public Protocol
{
public:
    int order;
    std::string length;
    long long size;
    std::string url;
    std::list<std::string> backup_url;

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(PlayUrlDurl, order, length, size, backup_url)

};

class PlayUrlDashVideo : public Protocol
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

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(PlayUrlDashVideo, id, base_url, backup_url, mimeType,\
        codecs, width, height, frameRate)
};

class PlayUrlDashDolby : public Protocol
{
public:
    std::list<PlayUrlDashVideo> audio;

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(PlayUrlDashDolby, audio)
};

class PlayUrlDash : public Protocol
{
public:
    std::list<PlayUrlDashVideo> video;
    std::list<PlayUrlDashVideo> audio;
    PlayUrlDashDolby dolby;

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(PlayUrlDash, video, audio, dolby)
};


class PlayUrlSupportFormat : public Protocol
{
public:
    int quality;
    std::string format;
    std::string new_description;
    std::string superscript;

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(PlayUrlSupportFormat, quality, format, new_description, superscript)

}; 

class PlayUrl : public Protocol
{
public:
    std::list<std::string> accept_description;
    std::list<int> accept_quality;
    std::list<PlayUrlDurl> durl;
    PlayUrlDash dash;
    std::list<PlayUrlSupportFormat> support_formats;

    
    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(PlayUrl, accept_description, accept_quality, durl, dash, support_formats)

};

// https://passport.bilibili.com/qrcode/getLoginUrl
class LoginUrl : public Protocol 
{
public:
    std::string oauthKey;
    std::string url;

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(LoginUrl, oauthKey, url)
};

class LoginUrlOrigin : public Protocol
{
public:
    LoginUrl data;
    bool status;

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(LoginUrlOrigin, data, status)
};


// https://passport.bilibili.com/qrcode/getLoginInfo
class LoginStatusData : public Protocol 
{
public:
    std::string url;

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(LoginStatusData, url)
};

class LoginStatusScanning : public Protocol 
{
public:
    LoginStatusData data;
    bool status;
    std::string message;

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(LoginStatusScanning, data, status, message)
};

class LoginStatus : public Protocol 
{
public:
    int code;
    bool status;
    std::string message;

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(LoginStatus, code, status, message)
};

class LoginStatusReady : public Protocol 
{
public:
    int code;
    LoginStatusData data;
    bool status;

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(LoginStatusReady, code, status, data)

};

} // namespace BiliApi




