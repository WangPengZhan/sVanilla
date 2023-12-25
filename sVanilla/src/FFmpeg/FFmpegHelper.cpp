#include <chrono>
#include <future>
#include <thread>

#include <QApplication>
#include <QDebug>
#include <QProcess>
#include <QString>

#include "FFmpegHelper.h"

namespace ffmpeg
{
constexpr char ffmpegCommand[] = "-i \"%1\" -i \"%2\" -acodec copy -vcodec copy -f mp4 \"%3\"";



FFmpegHelper::FFmpegHelper()
{
}

FFmpegHelper::~FFmpegHelper()
{
    closeFFmpeg();
}

void FFmpegHelper::megerVideo(const MergeInfo& mergeInfo)
{
    FFmpegHelper::megerVideo(mergeInfo, [] {}, [] {});
}

void FFmpegHelper::megerVideo(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc)
{
    FFmpegHelper::globalInstance().startFFpmegAsync(mergeInfo, errorFunc, finishedFunc);
}

FFmpegHelper& FFmpegHelper::globalInstance()
{
    static FFmpegHelper ffmpegHelper;
    return ffmpegHelper;
}

void FFmpegHelper::startFFpmegAsync(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc)
{
    // 检测上一次已经完成的合并移除
    m_futures.remove_if([](const std::future<bool>& future) {
        return future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    });

    // 启动新的合并
    std::future<bool> result = std::async(std::launch::async, [mergeInfo, errorFunc = std::move(errorFunc), finishedFunc = std::move(finishedFunc) ]() -> bool {
        QString ffmpegExeFilename;
        ffmpegExeFilename = QApplication::applicationDirPath() + "/ffmpeg.exe ";
        QString ffmpegArg(ffmpegCommand);
        ffmpegArg = ffmpegArg.arg(mergeInfo.audio.c_str()).arg(mergeInfo.video.c_str()).arg(mergeInfo.destionVideo.c_str());
        qDebug() << ffmpegArg;

        QProcess process;
        process.start(ffmpegExeFilename, ffmpegArg.split("-"));
        process.waitForFinished(-1);

        if (finishedFunc)
        {
            finishedFunc();
        }

        return true;
    });
    m_futures.push_back(std::move(result));
}

void FFmpegHelper::closeFFmpeg()
{
    for (auto& future : m_futures)
    {
        future.wait();
    }
    m_futures.clear();
}

} // namespace ffmpeg
