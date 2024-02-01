#include "BilibiliUtils.h"

std::string BiliApi::GetMixinKey(const std::string& orig)
{
    std::string result;
    for (const int i : mixinKeyEncTab) {
        result += orig[i];
    }
    return result.substr(0, 32);

}
CNetWork::ParamType& BiliApi::encWbi(CNetWork::ParamType& param, const std::string& mixin_key){
    const time_t curr_time = time(nullptr);
    param["wts"] = std::to_string(curr_time);
    for (const auto& [fst, snd] : param) {
        std::string value = snd;
        value.erase(std::remove_if(value.begin(), value.end(), [](char c) {
            return c == '!' || c == '\'' || c == '(' || c == ')' || c == '*';
        }), value.end());
        param[fst] = value;
    }

    std::string query;
    for (const auto& p : param) {
        if (!query.empty()) {
            query += "&";
        }
        query += p.first + "=" + p.second;
    }
    //
    // std::string wbi_sign;
    // unsigned char md5_digest[MD5_DIGEST_LENGTH];
    // MD5_CTX md5_ctx;
    // MD5_Init(&md5_ctx);
    // MD5_Update(&md5_ctx, (query + mixin_key).c_str(), (query + mixin_key).size());
    // MD5_Final(md5_digest, &md5_ctx);
    // for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
    //     char hex[3];
    //     sprintf(hex, "%02x", md5_digest[i]);
    //     wbi_sign += hex;
    // }
    // param["w_rid"] = wbi_sign;
}