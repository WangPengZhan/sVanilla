#pragma once
#include <string>

struct CoverInfo
{
    std::string url;
    std::string fileName;
    std::string path;
    int index;
};

bool downloadCoverImage(const CoverInfo& coverInfo);