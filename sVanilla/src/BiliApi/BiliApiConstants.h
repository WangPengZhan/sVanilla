#pragma once
#include <string>

namespace biliapi
{
constexpr const char* apiBase = "https://api.bilibili.com/";
constexpr const char* passBase = "https://passport.bilibili.com/";

namespace PassportURL
{
inline const std::string QRCode = std::string(passBase) + "x/passport-login/web/qrcode/generate";
inline const std::string LoginStatus = std::string(passBase) + "x/passport-login/web/qrcode/poll";
inline const std::string WebNav = std::string(apiBase) + "x/web-interface/nav";
}  // namespace PassportURL

namespace VideoURL
{
inline const std::string View = std::string(apiBase) + "x/web-interface/view";
inline const std::string Playurl = std::string(apiBase) + "x/player/wbi/playurl";
}  // namespace VideoURL

namespace Headers
{
inline const std::string DefaultReferer = "Referer: https://www.bilibili.com";
inline const std::string PassportReferer = "Referer: https://passport.bilibili.com/";
}  // namespace Headers
}  // namespace biliapi