#pragma once
#include <QObject>

#include "Download/DownloadStatusThread.h"
class DownloadManager final : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject* parent = nullptr);

    void addItem(std::shared_ptr<download::AbstractDownloader> downloader);

private:
    download::DownloadStatusThread m_thread;
};