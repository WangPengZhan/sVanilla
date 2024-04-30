#pragma once
#include <QObject>
#include <QString>

#include <list>
#include <memory>
#include <string>

#include "Download/AbstractDownloader.h"

Q_DECLARE_METATYPE(download::DownloadInfo)
struct VideoInfoFull;

class UiDownloader : public QObject, public download::AbstractDownloader
{
    Q_OBJECT
public:
    UiDownloader(std::shared_ptr<AbstractDownloader> downloader, QObject* parent = nullptr);
    ~UiDownloader();

    void setRealDownloader(std::shared_ptr<AbstractDownloader> realDownloader);
    std::shared_ptr<AbstractDownloader> realDownloader();

    void setFileName(const std::string& filename);
    const std::string& filename() const;
    void setVideoInfoFull(std::shared_ptr<VideoInfoFull> videoInfoFull);
    std::shared_ptr<VideoInfoFull> videoINfoFull() const;

    void start() override;
    void stop() override;
    void pause() override;
    void resume() override;
    void downloadStatus() override;
    void finish() override;

    void setGuid(std::string guid);

signals:
    void finished(QString msg);
    void statusChanged();
    void update(download::DownloadInfo downloadInfo, QString msg);

private:
    std::shared_ptr<download::AbstractDownloader> m_realDownloader;
    std::string m_filename;
    std::shared_ptr<VideoInfoFull> m_videoInfoFull;
};
