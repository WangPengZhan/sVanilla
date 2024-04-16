#pragma once
#include "DownloadingListWidget.h"
#include "Aria2Net/Protocol/Protocol.h"
#include "Download/DownloadStatusThread.h"

#include <unordered_set>

class DownloadManager final : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject* parent = nullptr);

    void addItem(std::shared_ptr<download::AbstractDownloader> downloader);

private:
    download::DownloadStatusThread m_thread;
};