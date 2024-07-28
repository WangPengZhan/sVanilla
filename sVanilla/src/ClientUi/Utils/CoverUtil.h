#pragma once
#include <string>

struct CoverInfo
{
    std::string url;
    std::string fileName;
    std::string path;
};

bool downloadCoverImage(const CoverInfo& coverInfo);
