#pragma once
#include <string>

namespace BiliApi
{
constexpr const char* apiBase = "https://api.bilibili.com/";
constexpr const char* passBase = "https://passport.bilibili.com/";

namespace PassportURL
{
inline std::string QRCode = std::string(passBase) + "x/passport-login/web/qrcode/generate";
inline  std::string LoginStatus = std::string(passBase) + "x/passport-login/web/qrcode/poll";
inline  std::string WebNav = std::string(apiBase) + "x/web-interface/nav";
}  // namespace Passport
// constexpr char videoViewUrl[] = "https://api.bilibili.com/x/web-interface/view";

}  // namespace BiliApi