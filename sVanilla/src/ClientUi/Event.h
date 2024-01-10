#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
#include <QObject>
#include <mutex>
#include "Aria2Net/Protocol/Protocol.h"

class Event final : public QObject {
    Q_OBJECT
public:
    static Event*getInstance();

public:
    explicit Event(QObject *parent = nullptr);
    Event&operator=(Event&) = delete;
    Event(Event&) = delete;

private:
    static Event*m_appEvent_ptr;
    static std::mutex m_mutex;

public slots:
signals:
    // (ui -> core)
    void BarBtnClick(int index);
    void UpdateTheme(int theme);
    void AddUri(std::list<std::string> uri);
    void AddDownloadTask(const std::string& gid);
    void IntervalUpdateDownloadStatus();
    voud OnDownloadCurrent(bool isCurrent);


    // (core -> ui)
    void updateAria2Version(std::shared_ptr<aria2net::AriaVersion> version);
    void updateDownloadStatus(std::shared_ptr<aria2net::AriaTellStatus> status);
    void updateMsg(std::string msg);

};

#pragma clang diagnostic pop