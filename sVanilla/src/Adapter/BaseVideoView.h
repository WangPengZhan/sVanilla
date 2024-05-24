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
    std::string PublishDate;
    std::string PlayListTitle;
};

using VideoView = std::vector<BaseVideoView>;

using Views = std::vector<std::shared_ptr<BaseVideoView>>;
}  // namespace Adapter
