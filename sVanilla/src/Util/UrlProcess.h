#pragma once

#include <QString>

namespace util
{

// 解析输入的字符串<para/>
// 支持的格式有：<para/>
// av号：av170001, AV170001, https://www.bilibili.com/video/av170001 <para/>
// BV号：BV17x411w7KC, https://www.bilibili.com/video/BV17x411w7KC, https://b23.tv/BV17x411w7KC <para/>
// 番剧（电影、电视剧）ss号：ss32982, SS32982, https://www.bilibili.com/bangumi/play/ss32982 <para/>
// 番剧（电影、电视剧）ep号：ep317925, EP317925, https://www.bilibili.com/bangumi/play/ep317925 <para/>
// 番剧（电影、电视剧）md号：md28228367, MD28228367, https://www.bilibili.com/bangumi/media/md28228367 <para/>
// 课程ss号：https://www.bilibili.com/cheese/play/ss205 <para/>
// 课程ep号：https://www.bilibili.com/cheese/play/ep3489 <para/>
// 收藏夹：ml1329019876, ML1329019876, https://www.bilibili.com/medialist/detail/ml1329019876, https://www.bilibili.com/medialist/play/ml1329019876/ <para/>
// 用户空间：uid928123, UID928123, uid:928123, UID:928123, https://space.bilibili.com/928123

static const std::string WwwUrl = "https://www.bilibili.com";
static const std::string ShareWwwUrl = "https://www.bilibili.com/s";
static const std::string ShortUrl = "https://b23.tv/";
static const std::string MobileUrl = "https://m.bilibili.com";
static const std::string SpaceUrl = "https://space.bilibili.com";

static const std::string VideoUrl = WwwUrl + "/video/";
static const std::string BangumiUrl = WwwUrl + "/bangumi/play/";
static const std::string BangumiMediaUrl = WwwUrl + "/bangumi/media/";
static const std::string CheeseUrl = WwwUrl + "/cheese/play/";
static const std::string FavoritesUrl1 = WwwUrl + "/medialist/detail/";
static const std::string FavoritesUrl2 = WwwUrl + "/medialist/play/";

class UrlProcess
{
public:
    static bool IsUrl(const QString& text);
    static bool IsIntId(const QString& text);
    static bool IsBvId(const QString& text);
    static bool IsBvUrl(const QString& text);
    static QString DeleteUrlParam(const QString& url);
    static QString GetId(const QString& text, const QString& baseUrl);
    static QString GetVideoId(const QString& text);
    static QString EnableHttps(const QString& url);
};

static constexpr char splicalChars[] = "?“”/\\<>*|:";

class FileHelp
{
public:
    static QString RemoveSpicalChar(const QString& path);
};

} // namespace util


