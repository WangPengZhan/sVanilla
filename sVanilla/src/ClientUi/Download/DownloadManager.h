#pragma once
#include "DownloadingListWidget.h"
#include "Aria2Net/Protocol/Protocol.h"
#include <QTimer>
#include <unordered_set>

class AbstractDownloader
{
public:

    enum Status
    {
        Ready,
        Downloading,
        Paused,
        Stopped,
        Finished,
        Error
    };

    explicit AbstractDownloader()= default;
    ~AbstractDownloader() = default;

    void updateStatus(const std::shared_ptr<aria2net::AriaTellStatus>& status);
    [[nodiscard]] Status status() const;
    [[nodiscard]] std::shared_ptr<aria2net::AriaTellStatus> getStatus() const;

private:
    std::shared_ptr<aria2net::AriaTellStatus> aria2Status;


protected:
    Status m_status = Downloading;
};


class DownloadManager final : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject* parent = nullptr);
    void addDownloader(const std::string& gid);

    std::unordered_set<std::string> neededUpdate;
    std::unordered_map<std::string, std::shared_ptr<AbstractDownloader>> downloaders;
    aria2net::AriaError updateDownloaderStatus(const std::shared_ptr<aria2net::AriaTellStatus>& status);
signals:
    void toRuquestStatus(const std::string& gid);
public slots:
    void updateDownloadStatusInterval();

private:
    QTimer* timer;
    void checkStatus(const std::string& gid);
};