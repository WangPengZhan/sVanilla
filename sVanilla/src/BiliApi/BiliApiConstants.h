#pragma once
#include <string>

namespace biliapi
{
inline const std::string apiBase = "https://api.bilibili.com/";
inline const std::string passBase = "https://passport.bilibili.com/";

namespace PassportURL
{
inline const std::string QRCode = passBase + "x/passport-login/web/qrcode/generate";
inline const std::string LoginStatus = passBase + "x/passport-login/web/qrcode/poll";
inline const std::string WebNav = apiBase + "x/web-interface/nav";
inline const std::string Logout = passBase + "login/exit/v2";
inline const std::string History = apiBase + "x/web-interface/history/cursor";
}  // namespace PassportURL

namespace VideoURL
{
inline const std::string View = apiBase + "x/web-interface/view";
inline const std::string Playurl = apiBase + "x/player/wbi/playurl";
}  // namespace VideoURL

namespace Headers
{
inline const std::string DefaultReferer = "Referer: https://www.bilibili.com";
inline const std::string PassportReferer = "Referer: https://passport.bilibili.com/";
}  // namespace Headers
}  // namespace biliapi