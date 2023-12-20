#include <vector>
#include <chrono>
#include <thread>
#include <map>
#include <any>

#include <gtest/gtest.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>

#include <QDir>
#include <QApplication>

#include "Aria2Net/AriaServer/AriaServer.h"
#include "Aria2Net/AriaClient/AriaClient.h"
#include "BiliApi/BilibiliClient.h"
#include "BiliApi/BiliApi.h"
#include "FFmpeg/FFmpegHelper.h"

TEST(Aria2Net_Test, STARTSERVER)
{
    spdlog::rotating_logger_mt<spdlog::async_factory>("Aria2Net", "log/Aria2Net.log", 1024 * 1024 * 10, 100);
}

TEST(Aria2Net_Test, AriaClient)
{
    using namespace aria2net;
    using namespace BiliApi;

    int a = 0;
    QApplication app(a, nullptr);
    aria2net::AriaServer server;
    server.StartServerAsync();

    auto biliClient = BiliApi::BilibiliClient::globalClient();
    std::string bvid = "BV1zv4y1T7NT";

    BiliApi::VideoView videoView = biliClient.GetVideoView(bvid);
    BiliApi::PlayUrl playUrl = biliClient.GetPlayUrl(videoView.cid, 125, videoView.bvid);

    std::list<std::string> video_urls;
    std::list<std::string> audio_urls;
    auto videos = playUrl.dash.video;
    for (const auto &video : videos)
    {
        video_urls.push_back(video.base_url);
    }
    auto audioes = playUrl.dash.audio;
    for (const auto &audio : audioes)
    {
        audio_urls.push_back(audio.base_url);
    }

    QDir dir("Download");
    QString dirPath = dir.absolutePath();
    if (!dir.exists())
    {
        dir.mkpath(dirPath);
    }

    aria2net::AriaSendOption ariaSendOption;
    ariaSendOption.dir = dirPath.toLocal8Bit().toStdString();
    ariaSendOption.out = (bvid + ".mp4");

    aria2net::AriaClient &ariaClient = aria2net::AriaClient::GetInstance();
    auto ariaAddUriVideo = ariaClient.AddUriAsync(video_urls, ariaSendOption);
    ariaSendOption.out = (bvid + ".mp3");
    auto ariaAddUriAudio = ariaClient.AddUriAsync(audio_urls, ariaSendOption);

    auto future = std::async(std::launch::async, [&, ariaAddUriVideo, ariaAddUriAudio]()
                             {
        bool videoOk = false, audioOk = false;
        while (true)
        {
            std::string str = ariaAddUriVideo.result;

            auto status = ariaClient.TellStatus(str);
            if (status.result.status == "complete")
            {
                videoOk = true;
            }

            str = ariaAddUriAudio.result;
            status = ariaClient.TellStatus(str);
            if (status.result.status == "complete")
            {
                audioOk = true;
            }

            if (audioOk && videoOk)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                FFmpegHelper::MegerVideo(dirPath.toStdString() + "/" + bvid + ".mp3",
                    dirPath.toStdString() + "/" + bvid + ".mp4",
                    dirPath.toStdString() + "/" + bvid + "all.mp4");
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        } });

    future.get();
}
