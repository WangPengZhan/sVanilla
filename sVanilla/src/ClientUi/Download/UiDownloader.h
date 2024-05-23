#pragma once
#include <QObject>
#include <QString>

#include <list>
#include <memory>
#include <string>

#include "Download/AbstractDownloader.h"

Q_DECLARE_METATYPE(download::DownloadInfo)
struct VideoInfoFull;
namespace sqlite
{
class StorageManager;
}

class UiDownloader : public QObject, public download::AbstractDownloader
{
    Q_OBJECT
public:
    UiDownloader(std::shared_ptr<AbstractDownloader> downloader, std::shared_ptr<VideoInfoFull> videoInfoFull, QObject* parent = nullptr);
    ~UiDownloader();

    void setRealDownloader(const std::shared_ptr<AbstractDownloader>& realDownloader);
    std::shared_ptr<AbstractDownloader> realDownloader();

    void setFileName(const std::string& filename);
    const std::string& filename() const;
    std::shared_ptr<VideoInfoFull> videoINfoFull() const;

    void start() override;
    void stop() override;
    void pause() override;
    void resume() override;
    void downloadStatus() override;
    void finish() override;

signals:
    void finished(QString msg);
    void statusChanged(download::AbstractDownloader::Status status);
    void update(download::DownloadInfo downloadInfo, QString msg);

private:
    void createDbDownloadingItem();
    void updateDbStatus();
    void deleteDbDownloadingItem();
    void ctreateDbFinishedItem();

private:
    std::shared_ptr<download::AbstractDownloader> m_realDownloader;
    std::string m_filename;
    std::shared_ptr<VideoInfoFull> m_videoInfoFull;
    sqlite::StorageManager& m_storageManager;
};
