#pragma once
#include <QObject>

#include <memory>

namespace download
{
class AbstractDownloader;
}
class DownloadManager final : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject* parent = nullptr);

    void addItem(std::shared_ptr<download::AbstractDownloader> downloader);
};
