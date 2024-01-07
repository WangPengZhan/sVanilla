#pragma once
#include <QDir>
#include <QMutex>
#include <QObject>
#include <QSharedPointer>
#include <QVariant>


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
    void BarBtnClick(int index);
    void SearchBtnClick();
    void AccountBtnClick();
    void StackedPageChanged(int index);

    void updateAria2Status(std::string status);
    void updateVersion(std::string version);
    void updateFeatures(std::string features);

    void addSingleUri(std::string uri);
    void ClipboardBtnClick();
};
