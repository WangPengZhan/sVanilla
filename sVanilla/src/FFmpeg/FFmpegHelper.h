#pragma once

#include <string>

// #include <Windows.h>

class FFmpegHelper
{
public:
    FFmpegHelper();
    ~FFmpegHelper();
    static bool MergeVideo(const std::string& audio, const std::string& video, const std::string& targetVideo);

    static void StartFFpmegAsync(const std::string& audio, const std::string& video, const std::string& targetVideo);
    void CloseFFmpeg();
};
