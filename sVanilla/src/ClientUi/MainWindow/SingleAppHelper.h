#pragma once
#include <QObject>
#include <QString>

class QWidget;
class QLocalServer;
class SingleAppHelper : public QObject
{
    Q_OBJECT
public:
    SingleAppHelper();
    ~SingleAppHelper();

    void setMainWidget(QWidget* widget);
    QWidget* mainWidget() const;

    bool isHaveInstance() const;

private:
    void tryConnectLocaled();
    void newLocalConnection();

private:
    QLocalServer* m_pLocalServer;
    QWidget* m_pMainWidget;
    bool m_bHaveInstance;
    static QString m_strServerName;
};
