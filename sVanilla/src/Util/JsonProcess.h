#pragma once

// class nlohmann::json;

namespace util
{

class JsonProcess
{
public:
    static void removeNullValues(nlohmann::json& jsonObj);
};

}  // namespace util