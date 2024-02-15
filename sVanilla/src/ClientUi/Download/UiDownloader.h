#pragma once
#include <QObject>

#include <list>
#include <memory>

#include "Download/AbstractDownloader.h"


class UiDownloader : public QObject, public download::AbstractDownloader
{
    Q_OBJECT
public:
    UiDownloader(std::shared_ptr<download::AbstractDownloader> downloader, QObject* parent = nullptr);
    ~UiDownloader();

    void setRealDownloader(std::shared_ptr<download::AbstractDownloader> realDownloader);
    std::shared_ptr<download::AbstractDownloader> realDownloader();

    void setFileName(const std::string& filename);
    const std::string& filename() const;

    void start() override;
    void stop() override;
    void pause() override;
    void resume() override;
    void downloadStatus() override;
    void finish() override;
    // in slot changed status and DownloadStatusTread to do in real

signals:
    void finished(QString msg);
    void statusChanged();
    void update(QString msg);

private:
    std::shared_ptr<download::AbstractDownloader> m_realDownloader;
    std::string m_filename;
};


