#pragma once
#include <QObject>
#include <QString>

#include <list>
#include <memory>
#include <string>

#include "Download/AbstractDownloader.h"

Q_DECLARE_METATYPE(download::DownloadInfo)

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

    void start() override;
    void stop() override;
    void pause() override;
    void resume() override;
    void downloadStatus() override;
    void finish() override;

signals:
    void finished(QString msg);
    void statusChanged();
    void update(download::DownloadInfo downloadInfo, QString msg);

private:
    std::shared_ptr<download::AbstractDownloader> m_realDownloader;
    std::string m_filename;
};
