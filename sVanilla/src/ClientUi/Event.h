#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
#pragma once
#include <QDir>
#include <QMutex>
#include <QObject>
#include <QSharedPointer>
#include <QVariant>
#include "Aria2Net/Protocol/Protocol.h"

class Event : public QObject {
    Q_OBJECT
public:
    static Event*getInstance();

public:
    explicit Event(QObject *parent = nullptr);
    Event&operator=(Event&) = delete;
    Event(Event&) = delete;

private:
    static Event*m_appEvent_ptr;
    static QMutex m_mutex;

public slots:
signals:
    // (ui -> core)
    void BarBtnClick(int index);
    void UpdateTheme(int theme);
    void ClipboardBtnClick();
    void AddUri(std::list<std::string> uri);


    // (core -> ui)
    void updateAria2Version(std::shared_ptr<aria2net::AriaVersion> version);
    void updateMsg(std::string msg);
    void updateAria2Status(std::string status);
    void updateVersion(std::string version);
    void updateFeatures(std::string features);

};

#pragma clang diagnostic pop