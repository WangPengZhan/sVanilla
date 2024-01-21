#include <vector>
#include <chrono>
#include <thread>
#include <map>
#include <any>

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

int main(int argc, char* argv[])
{
    spdlog::rotating_logger_mt<spdlog::async_factory>("Aria2Net", "log/Aria2Net.log", 1024 * 1024 * 10, 100);
    spdlog::rotating_logger_mt<spdlog::async_factory>("Network", "log/Network.log", 1024 * 1024 * 10, 100);

    using namespace aria2net;
    using namespace BiliApi;
    using namespace ffmpeg;

    int a = 0;
    QApplication app(a, nullptr);
    AriaServer server;
    server.startServerAsync();

    auto biliClient = BilibiliClient::globalClient();
    std::string bvid = "BV1zv4y1T7NT";

    VideoView videoView = biliClient.GetVideoView(bvid);
    PlayUrl playUrl = biliClient.GetPlayUrl(videoView.cid, 64, videoView.bvid);

    std::list<std::string> video_urls;
    std::list<std::string> audio_urls;
    auto videos = playUrl.dash.video;
    for (const auto& video : videos)
    {
        video_urls.push_back(video.base_url);
    }
    auto audioes = playUrl.dash.audio;
    for (const auto& audio : audioes)
    {
        audio_urls.push_back(audio.base_url);
    }

    QDir dir("Download");
    QString dirPath = dir.absolutePath();
    if (!dir.exists())
    {
        dir.mkpath(dirPath);
    }

    AriaSendOption ariaSendOption;
    ariaSendOption.dir = dirPath.toLocal8Bit().toStdString();
    ariaSendOption.out = (bvid + ".mp4");

    AriaClient& ariaClient = AriaClient::globalClient();
    auto ariaAddUriVideo = ariaClient.AddUriAsync(video_urls, ariaSendOption);
    ariaSendOption.out = (bvid + ".mp3");
    auto ariaAddUriAudio = ariaClient.AddUriAsync(audio_urls, ariaSendOption);

    // auto ariaAddUriVideo = ariaClient.AddUriAsync({"https://dldir1.qq.com/qqfile/qq/PCQQ9.7.1/QQ9.7.1.28940.exe"}, ariaSendOption);

    auto future = std::async(std::launch::async, [&, ariaAddUriVideo, ariaAddUriAudio]() {
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
                FFmpegHelper::mergeVideo(
                    {dirPath.toStdString() + "/" + bvid + ".mp3", dirPath.toStdString() + "/" + bvid + ".mp4", dirPath.toStdString() + "/" + bvid + "all.mp4"});
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    });

    future.get();

    return app.exec();
}
