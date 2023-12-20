#include <chrono>
#include <future>
#include <thread>

#include <QApplication>
#include <QDebug>
#include <QProcess>
#include <QString>

#include "FFmpegHelper.h"

namespace
{
constexpr char ffmpegCommand[] = "-i \"%1\" -i \"%2\" -acodec copy -vcodec copy -f mp4 \"%3\"";
}

FFmpegHelper::FFmpegHelper()
{
}

FFmpegHelper::~FFmpegHelper()
{
    CloseFFmpeg();
}

bool FFmpegHelper::MegerVideo(const std::string& audio, const std::string& video, const std::string& destionVideo)
{
    FFmpegHelper ffmpegHelper;
    ffmpegHelper.StartFFpmegAsync(audio, video, destionVideo);
    return true;
}

void FFmpegHelper::StartFFpmegAsync(const std::string& audio, const std::string& video, const std::string& destionVideo)
{
    std::future<bool> result = std::async(std::launch::async, [&]() -> bool {
        QString ffmpegExeFilename;
        ffmpegExeFilename = QApplication::applicationDirPath() + "/ffmpeg.exe ";
        QString ffmpegArg(ffmpegCommand);
        ffmpegArg = ffmpegArg.arg(audio.c_str()).arg(video.c_str()).arg(destionVideo.c_str());
        qDebug() << ffmpegArg;

        QProcess process;
        process.start(ffmpegExeFilename, ffmpegArg.split("-"));
        process.waitForFinished(-1);

        return true;
    });
}

void FFmpegHelper::CloseFFmpeg()
{
    // to do
}
