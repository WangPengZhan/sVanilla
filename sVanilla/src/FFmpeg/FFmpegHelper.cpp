#include <chrono>
#include <future>
#include <thread>
#include <QStandardPaths>
#include <QApplication>
#include <QDebug>
#include <QProcess>
#include <QString>

#include "FFmpegHelper.h"
#include "Aria2Net/AriaLog.h"

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

bool FFmpegHelper::MergeVideo(const std::string& audio, const std::string& video, const std::string& targetVideo)
{
    FFmpegHelper ffmpegHelper;
    FFmpegHelper::StartFFpmegAsync(audio, video, targetVideo);
    return true;
}

void FFmpegHelper::StartFFpmegAsync(const std::string& audio, const std::string& video, const std::string& targetVideo)
{
    std::future<bool> result = std::async(std::launch::async, [&]() -> bool {
        QString executablePath = QApplication::applicationDirPath();
        QString ffmpegExecutable = QStandardPaths::findExecutable("ffmpeg", QStringList() << executablePath);
        QString ffmpegArg(ffmpegCommand);
        ffmpegArg = ffmpegArg.arg(audio.c_str()).arg(video.c_str()).arg(targetVideo.c_str());
        qDebug() << ffmpegArg;

        QProcess ffmpegProcess;
        ffmpegProcess.setProgram(ffmpegExecutable);
        ffmpegProcess.setArguments(ffmpegArg.split("-"));
        ffmpegProcess.start();
        ffmpegProcess.waitForFinished(-1);

        if (ffmpegProcess.exitStatus() == QProcess::NormalExit && ffmpegProcess.exitCode() == 0)
        {
            // ffmpeg正常结束且返回值为0，表示执行成功
            return true;
        }
        else
        {
            // 否则，表示ffmpeg执行失败
            std::string errorString = ffmpegProcess.errorString().toStdString();
            int exitCode = ffmpegProcess.exitCode();
            ARIA_LOG_ERROR("{}:Error starting ffmpeg process: {}", exitCode, errorString);
            return false;
        }
    });
}

void FFmpegHelper::CloseFFmpeg()
{
    // to do
}
