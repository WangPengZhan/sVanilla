#pragma once
#include <QObject>

class DownloadManager final : public QObject
{
    Q_OBJECT
    typedef std::list<std::string> List;

public:
    explicit DownloadManager(QObject* parent = nullptr);
    void addDownloadGID(const std::string& gid);

    List downloadGIDs;
};