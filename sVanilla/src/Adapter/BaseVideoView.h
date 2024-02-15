#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace Adapter
{

struct BaseVideoView {
    std::string Identifier;
    std::string AlternateId;
    std::string VideoId;
    std::string Title;
    std::string Publisher;
    std::string Cover;
    std::string Duration;
    std::string Description;
};

using VideoView = std::vector<BaseVideoView>;
}  // namespace Adapter
