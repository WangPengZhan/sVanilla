#pragma once


namespace util
{

class nlohmann::json;
class JsonProcess
{
public:
    static void removeNullValues(nlohmann::json& jsonObj);
};

} // namespace util