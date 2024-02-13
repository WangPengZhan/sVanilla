#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace Adapter
{
class Protocol
{
public:
    Protocol() = default;
    virtual ~Protocol() = default;
    virtual std::string toString() = 0;
};
class VideoView : public Protocol
{
public:
    std::string Identifier;
    std::string AlternateId;
    std::string VideoId;
    std::string Title;
    std::string Duration;
    std::string Publisher;
    std::string Cover;
    std::string Description;

    std::string toString() override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump(4);
    }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(VideoView, Identifier, AlternateId, VideoId, Title, Duration, Publisher, Cover, Description)
};
}  // namespace Adapter
