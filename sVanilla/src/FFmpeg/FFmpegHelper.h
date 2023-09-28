#pragma once

#include <string>

#include <Windows.h>

class FFmpegHelper
{
public:
    FFmpegHelper();
    ~FFmpegHelper();
    static bool MegerVideo(const std::string& audio, const std::string& video, const std::string& destionVideo);

    void StartFFpmegAsync(const std::string& audio, const std::string& video, const std::string& destionVideo);
    void CloseFFmpeg();
};
