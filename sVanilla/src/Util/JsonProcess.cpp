#include <nlohmann/json.hpp>

#include <iostream>

#include "JsonProcess.h"

void util::JsonProcess::removeNullValues(nlohmann::json& jsonObj)
{
    if (jsonObj.is_null())
    {
        return;
    }

    if (jsonObj.is_object())
    {
        for (auto it = jsonObj.begin(); it != jsonObj.end();)
        {
            if (it.value().is_null())
            {
                std::cout << "key: " << it.key() << "value" << it.value().dump();
                it = jsonObj.erase(it);
            }
            else
            {
                removeNullValues(it.value());
                ++it;
            }
        }
    }
    else if (jsonObj.is_array())
    {
        for (auto& element : jsonObj)
        {
            removeNullValues(element);
        }
    }
}